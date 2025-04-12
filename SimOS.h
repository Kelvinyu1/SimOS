#ifndef SIM_OS
#define SIM_OS

#include <queue>
#include <vector>
#include <iostream>
#include <string>

struct Process {
  unsigned long long itemSize;
  int priorty;
  int PID;
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
    int current_process;
    MemoryItem os_item;
    Process os_process;
    int numberOfDIsks;
    unsigned long long amountofRam;
    unsigned long long sizeOfOS;
     
};



#endif
