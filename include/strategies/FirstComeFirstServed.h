#pragma once
#include "SchedulerStrategy.h"

class FirstComeFirstServed : public SchedulerStrategy
{
   public:
    std::optional<size_t> selectNext(std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>>& queues,
                                     const std::vector<PCB>& process_pool,
                                     int currentTime) override
    {
        // Store the first arrived process in the highest priority
        // maybe just take queues[0] ?
        size_t idx{0};
        bool found = false;
        for (auto prio = 1; prio <= queues.size() - 1; ++prio)
        {
            if (!queues.empty())
            {
                std::cout << "Choose from FCFS: " << queues[process_pool[prio].getPid()].front()
                          << std::endl;
                idx = queues[prio].front();
                found = true;
            }
            if (found)
            {
                for (auto& q : queues)
                {
                    q.remove(idx);
                    break;
                }
                return idx;
            }
        }

        return std::nullopt;
    }

    // runs for whole remaining time or until i/o
    int getTimeSlice(const PCB& process, int default_quantum) const override
    {
        return process.getRemainingTime();
    }

    // doesnt preempt for newly arrived processes
    bool shouldPreempt(const PCB& current_process, const PCB& new_process) const override
    {
        return false;
    }

    // only has one priority level (1)
    int getReinsertionPolicy(const PCB& process) const override
    {
        return 1;
    }

    // doesnt use priority scheduling
    bool usesPriorityQueues() const override
    {
        return false;
    }

    // doesnt use aging
    bool usesAging() const override
    {
        return false;
    }

    std::string name() const override
    {
        return "First Come First Served Strategy";
    }

    std::string description() const override
    {
        return "Non-Preemptive FCFS: The first process to enter the queue, will run until finished "
               "or I/O Block";
    }
};