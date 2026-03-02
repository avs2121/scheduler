#pragma once
#include <optional>
#include <string>

#include "SchedulerClass.h"

class SchedulerStrategy
{
   public:
    virtual ~SchedulerStrategy() = default;
    virtual std::optional<size_t> selectNext(
        std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>>& queues,
        const std::vector<PCB>& process_pool) = 0;

    virtual std::string name() const = 0;
};