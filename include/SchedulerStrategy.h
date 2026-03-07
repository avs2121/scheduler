#pragma once
#include <optional>
#include <string>
#include <vector>

#include "Constants.h"
#include "PCB.h"
#include "ReadyQueue.h"

class SchedulerStrategy
{
   public:
    virtual ~SchedulerStrategy() = default;
    virtual std::optional<size_t> selectNext(
        std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>>& queues,
        const std::vector<PCB>& process_pool,
        int currentTime) = 0;

    // get the time slice for how long the process should run
    virtual int getTimeSlice(const PCB& process, int default_quantum) const = 0;

    // should the new process preempt the current process, called when new process arrives / becomes
    // ready
    virtual bool shouldPreempt(const PCB& current_process, const PCB& new_process) const = 0;

    // called after process finish quantum to determine where it goes (priority level)
    virtual int getReinsertionPolicy(const PCB& process) const = 0;

    virtual bool usesPriorityQueues() const = 0;

    // if algorithm uses aging
    virtual bool usesAging() const = 0;

    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
};