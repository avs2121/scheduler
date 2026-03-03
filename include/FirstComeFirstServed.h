#pragma once
#include "SchedulerStrategy.h"

class FirstComeFirstServed : SchedulerStrategy
{
   public:
    std::optional<size_t> selectNext(std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>>& queues,
                                     const std::vector<PCB>& process_pool) override
    {
        // Store the first arrived process in the highest priority
        // maybe just take queues[0] ?
        size_t idx{0};
        bool found = false;
        for (auto prio = 1; prio <= queues.size() - 1; ++prio)
        {
            if (!queues.empty())
            {
                std::cout << "Choose from FCFS: " << queues[prio].front() << std::endl;
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

    std::string name() const override
    {
        return "First Come First Served Strategy";
    }
};