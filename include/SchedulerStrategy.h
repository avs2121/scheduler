#pragma once
#include <iostream>
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

    virtual std::string name() const = 0;
};