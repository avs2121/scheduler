#pragma once
#include "SchedulerStrategy.h"

class FirstComeFirstServed : SchedulerStrategy
{
   public:
    std::optional<size_t> selectNext(std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>>& queues,
                                     const std::vector<PCB>& process_pool) override
    {
        // Store the first arrived process in the highest priority
        return std::nullopt;
    }

    std::string name() const override
    {
        return "First Come First Served Strategy";
    }
};