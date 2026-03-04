#pragma once
#include <memory>

#include "SchedulerStrategy.h"

class StrategyFactory
{
   public:
    static std::unique_ptr<SchedulerStrategy> createStrategy(const std::string& algo_name);
};