#pragma once
#include <deque>
#include <vector>

#include "PCB.h"

class IOManager
{
   public:
    IOManager(std::array<PCB, N>& process_pool);

    void enqueue(size_t idx);
    void updateIO();
    void processIO(int timeslice);
    void handleIOqueue();
    const std::vector<size_t>& getFinishedProcesses() const;
    const std::deque<size_t>& getQueue() const;

    // Query methods
    bool isEmpty() const;
    size_t size() const;
    bool containsPID(int pid) const;

    // Utility methods
    void clear();
    void clearFinished();
    int getMinRemainingIOTime() const;
    void printQueue() const;

   private:
    std::array<PCB, N>& process_pool;
    std::deque<size_t> IO_queue;
    std::vector<size_t> finished_IO;
};
