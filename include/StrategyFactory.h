#pragma once
#include <functional>
#include <memory>
#include <unordered_map>

#include "SchedulerStrategy.h"

class StrategyFactory
{
   public:
    static std::unique_ptr<SchedulerStrategy> createStrategy(const std::string& algo_name);
    static std::unique_ptr<SchedulerStrategy> createStrategy_map(const std::string& algo_name);

   private:
    static const std::unordered_map<std::string,
                                    std::function<std::unique_ptr<SchedulerStrategy>()>>
        strategy_map;
};