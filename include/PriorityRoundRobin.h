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

    std::string name() const override
    {
        return "Priority Round Robin Strategy";
    }
};