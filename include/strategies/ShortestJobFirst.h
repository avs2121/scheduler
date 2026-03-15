#pragma once

#include "SchedulerStrategy.h"

class ShortestJobFirst : public SchedulerStrategy
{
   public:
    std::optional<size_t> selectNext(std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>>& queues,
                                     const std::vector<PCB>& process_pool,
                                     int currentTime) override
    {
        // store the provisional process with the shortest remaining time while iterating through
        // all processes in that priority.

        size_t shortest_idx{0};
        int shortest_time = INT_MAX;
        bool found = false;

        for (auto prio = 1; prio <= queues.size() - 1; ++prio)
        {
            if (!queues[prio].empty())
            {
                size_t temp_idx = queues[prio].front();

                if (process_pool[temp_idx].getRemainingTime() < shortest_time)
                {
                    shortest_time = process_pool[temp_idx].getRemainingTime();
                    shortest_idx = temp_idx;
                    found = true;
                }
            }
        }
        std::cout << "Choose PID from SJF: " << queues[process_pool[shortest_idx].getPid()]
                  << std::endl;

        if (found)
        {
            for (auto& q : queues)
            {
                if (q.remove(shortest_idx))
                    break;
            }
            return shortest_idx;
        }
        return std::nullopt;  // absence of value -> no process to pop
    }

    // check in the default_quantum interval whether it should preempt
    int getTimeSlice(const PCB& process, int default_quantum) const override
    {
        return std::min(process.getRemainingTime(), default_quantum);
    }

    // should the new process preempt the current process, called when new process arrives / becomes
    // ready
    bool shouldPreempt(const PCB& current_process, const PCB& new_process) const override
    {
        return current_process.getRemainingTime() > new_process.getRemainingTime();
    }

    // called after process finish quantum to determine where it goes (priority level)
    int getReinsertionPolicy(const PCB& process, int time_elapsed) const override
    {
        return process.getPriority();
    }

    // uses priority scheduling
    bool usesPriorityQueues() const override
    {
        return true;
    }

    // uses aging
    bool usesAging() const override
    {
        return true;
    }

    std::string name() const override
    {
        return "Priority-based Shortest Job First Strategy";
    }

    std::string description() const override
    {
        return "Preemptive Priority-SJF: Always runs the process within the highest priority with "
               "the least remaining time";
    }
};