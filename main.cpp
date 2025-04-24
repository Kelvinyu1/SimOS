#include "SimOS.h"

int main() {
    SimOS sim(1, 64'000'000, 1'000'000);

    sim.NewProcess(10'000'000, 5); // PID 2

    sim.NewProcess(15'000'000, 4); // PID 3

    sim.NewProcess(5'000'000, 3); // PID 4

    if (sim.GetCPU() == 2) {
        sim.SimExit(); // terminate PID 2
    }
    std::vector<int> ready = sim.GetReadyQueue();
    sim.NewProcess(500'000, 10); // PID 5, should be right after OS memory

    sim.SimFork();

    sim.SimExit();

    sim.NewProcess(12'000'000, 6); // PID 6, should be at the end



    sim.NewProcess(500'000, 2); // PID 9

    std::cout << "Memory layout:\n";
    MemoryUse mem = sim.GetMemory();
    for (const auto& item : mem) {
        std::cout << "PID " << item.PID << " @ " << item.itemAddress
                  << " (" << item.itemSize << " bytes)\n";
    }

    return 0;
}

