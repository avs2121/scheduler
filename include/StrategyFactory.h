#pragma once
#include <functional>
#include <memory>
#include <unordered_map>

#include "SchedulerStrategy.h"
#include "strategies/FirstComeFirstServed.h"
#include "strategies/MultiLevelFeedbackQueue.h"
#include "strategies/PriorityRoundRobin.h"
#include "strategies/ShortestJobFirst.h"

class StrategyFactory
{
   public:
    static std::unique_ptr<SchedulerStrategy> createStrategy(const std::string& algo_name);
    static std::unique_ptr<SchedulerStrategy> createStrategy_map(std::string& algo_name);

   private:
    inline static const std::unordered_map<std::string,
                                           std::function<std::unique_ptr<SchedulerStrategy>()>>
        strategy_map = {{"rr", []() { return std::make_unique<PriorityRoundRobin>(); }},
                        {"fcfs", []() { return std::make_unique<FirstComeFirstServed>(); }},
                        {"sjf", []() { return std::make_unique<ShortestJobFirst>(); }},
                        {"mlfq", []() { return std::make_unique<MultiLevelFeedbackQueue>(); }}};
};