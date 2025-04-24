#ifndef SIM_OS
#define SIM_OS

#include <algorithm>
#include <optional>
#include <queue>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>

enum class ProcessState { READY, RUNNING, WAITING, ZOMBIE, TERMINATED};


struct Process {
  int priority;
  int PID;
  int parentPID;
  unsigned long long memorySize;
  std::vector<int> children;
  ProcessState state = ProcessState::READY;
};

struct FileReadRequest {
  int PID{0};
  std::string fileName{""};
};

struct MemoryItem {
  unsigned long long itemAddress;
  unsigned long long itemSize;
  int PID; 
};

using MemoryUse = std::vector<MemoryItem>;

constexpr int NO_PROCESS = -1;

struct Disk {
  int number;
  FileReadRequest currentRequest;
  std::queue<FileReadRequest> queue; 
};

class SimOS {
  public:
    SimOS(int numberOfDisks, unsigned long long amountofRAM, unsigned long long sizeOfOS);
    bool NewProcess(unsigned long long size, int priority);
    bool SimFork();
    void SimExit();
    void SimWait();
    void DiskReadRequest(int diskNumber, std::string fileName);
    void DiskJobCompleted(int diskNumber);
    int GetCPU();
    std::vector<int> GetReadyQueue();
    MemoryUse GetMemory();
    FileReadRequest GetDisk(int diskNumber);
    std::queue<FileReadRequest> GetDiskQueue(int diskNumber);


  private:
    std::optional<int> cpuPID;
    int nextPID;
    std::unordered_map<int, Process> processTable; 
    std::priority_queue<std::pair<int, int>> readyQueue;

    unsigned long long totalRAM;
    unsigned long long osSize; 
    MemoryUse memoryUse;

    std::unordered_map<int, std::vector<int>> zombieTable;
    std::unordered_map<int, bool> waitingTable;

    std::vector<Disk> disks;
    int numberOfDisks;

    void cascadeTerminate(int pid);
    void scheduleNext();
    
};



#endif
