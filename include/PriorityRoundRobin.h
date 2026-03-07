#pragma once
#include "SchedulerStrategy.h"

class PriorityRoundRobin : public SchedulerStrategy
{
   public:
    std::optional<size_t> selectNext(std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>>& queues,
                                     const std::vector<PCB>& process_pool,
                                     int currentTime) override
    {
        for (auto prio = 1; prio <= queues.size() - 1; ++prio)
        {
            if (!queues[prio].empty())
            {
                return queues[prio].pop();
            }
        }
        return std::nullopt;  // absence of value -> no process to pop
    }

    // return time quantum
    int getTimeSlice(const PCB& process, int default_quantum) const override
    {
        return default_quantum;
    }

    // only preempt if the new process is of higher priority
    bool shouldPreempt(const PCB& current_process, const PCB& new_process) const override
    {
        return current_process.getPriority() < new_process.getPriority();
    }

    // rr reinsert at priority level
    int getReinsertionPolicy(const PCB& process) const override
    {
        return process.getPriority();
    }

    // uses priority
    bool usesPriorityQueues() const override
    {
        return true;
    }

    // rr uses aging
    bool usesAging() const override
    {
        return true;
    }

    std::string name() const override
    {
        return "Priority Round Robin Strategy";
    }

    std::string description() const override
    {
        return "Multi-level priority with round-robin within each priority-level";
    }
};