#include "SimOS.h"

SimOS::SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned long long sizeOfOS) 
  : totalRAM(amountOfRAM), osSize(sizeOfOS), numberOfDisks(numberOfDisks), nextPID(2)
{
  Process osProcess {0, 1, 0, sizeOfOS, {}, ProcessState::RUNNING}; 
  processTable[1] = osProcess;
  cpuPID = 1;

  MemoryItem os {0, sizeOfOS, 1};
  memoryUse.push_back(os);

  disks.resize(numberOfDisks);
}

bool SimOS::NewProcess(unsigned long long size, int priority) {
  unsigned long long maxHoleSize = 0;
  unsigned long long bestAddress = 0;
  bool found = false;

  for (size_t i = 0; i + 1 < memoryUse.size(); i++) {
    unsigned long long holeStart = memoryUse[i].itemAddress + memoryUse[i].itemSize;
    unsigned long long holeEnd = memoryUse[i+1].itemAddress;
    unsigned long long holeSize = holeEnd - holeStart;

    std::cout << "when placing priority " << priority << " this hole has a size of " << holeSize << "\n";
  
    if (holeSize >= size) {
      if (holeSize > maxHoleSize) {
      maxHoleSize = holeSize;
      bestAddress = holeStart;
      found = true;
      } else if (holeSize == maxHoleSize && holeStart < bestAddress) {
        bestAddress = holeStart;
        found = true;
      }
      
    }
  }
    //check last spot 
    unsigned long long lastEnd = memoryUse.back().itemAddress + memoryUse.back().itemSize;
    unsigned long long holeSize = totalRAM - lastEnd;

    if (holeSize >= size) {
      if (holeSize > maxHoleSize) {
      maxHoleSize = holeSize;
      bestAddress = lastEnd;
      found = true;
      }
    }     

    if (!found) return false;

    int pid = nextPID++;

    Process newProcess { priority, pid, 0, size, {}, ProcessState::READY};
    processTable[pid] = newProcess;

    MemoryItem newMem {bestAddress, size, pid};
    auto insertPos = std::lower_bound(memoryUse.begin(), memoryUse.end(), newMem, 
      [](const MemoryItem& a, const MemoryItem& b) {
        return a.itemAddress < b.itemAddress;
      });
    memoryUse.insert(insertPos, newMem);

    //cpu scheduling, i think make this a helper function and the checking for memory allocation cus it's too long bruh

    if(!cpuPID.has_value()) {
      cpuPID = pid; 
      processTable[pid].state = ProcessState::RUNNING;
    } else {
      int currentPID = cpuPID.value();
      int currentPriority = processTable[currentPID].priority; 

      if (priority > currentPriority) {
        processTable[currentPID].state = ProcessState::READY;
        readyQueue.push({currentPriority, currentPID});

        cpuPID = pid;
        processTable[pid].state = ProcessState::RUNNING;
      } else {
        readyQueue.push({priority, pid});
      }
    }
  std::cout << "Placing process at: " << bestAddress << " (hole size: " << maxHoleSize << ")\n";    
  return true;
}

bool SimOS::SimFork() { 
  if (cpuPID.has_value()) {
    Process currProc = processTable[cpuPID.value()];
    return NewProcess(currProc.memorySize, currProc.priority);
  }

  return false;
}

void SimOS::SimExit() {
  if (!cpuPID.has_value() || cpuPID.value() == 1) return;

  int pid = cpuPID.value();
  cpuPID.reset();

  cascadeTerminate(pid);
  scheduleNext();
}

void SimOS::cascadeTerminate(int pid) {
  auto it = processTable.find(pid);
  if (it == processTable.end()) return;

  Process& killProc = it->second;

  for (int childPID: killProc.children) {
    cascadeTerminate(childPID);
  }

  auto it2 = std::find_if(memoryUse.begin(), memoryUse.end(), 
    [pid](const MemoryItem& item) {return item.PID == pid; });

  if (it2 != memoryUse.end()) {
    memoryUse.erase(it2); 
  }

  int parentPid = killProc.parentPID; 
  bool isWaiting = waitingTable[parentPid]; // the parent, but im lazy to call it ParentisWaiting

  if (isWaiting) {
    Process& parent = processTable[parentPid];
    parent.state = ProcessState::READY;
    waitingTable[parentPid] = false;
    readyQueue.push({parent.priority, parentPid});
  } else {
    zombieTable[parentPid].push_back(pid);
    killProc.state = ProcessState::ZOMBIE;
    return;
  }

  killProc.state = ProcessState::TERMINATED;
}

void SimOS::scheduleNext() { 
  while(!readyQueue.empty()) {
    auto [priority, pid] = readyQueue.top();
    readyQueue.pop();

    if (processTable[pid].state == ProcessState::READY) { 
      cpuPID = pid;
      processTable[pid].state = ProcessState::RUNNING;
      return;
    }
  }
  cpuPID.reset(); //edge case if shostak wants to schedule a next process but none exist
}

void SimOS::SimWait() { 
  
}

int SimOS::GetCPU() {
  return cpuPID.value_or(NO_PROCESS);
}

std::vector<int> SimOS::GetReadyQueue() {
  std::priority_queue<std::pair<int, int>> copy = readyQueue;
  std::vector<int> readyPIDS;
  while (!copy.empty()) {
    readyPIDS.push_back(copy.top().second);
    copy.pop();
  }

  return readyPIDS;
}

MemoryUse SimOS::GetMemory() { 
  return memoryUse;
}
