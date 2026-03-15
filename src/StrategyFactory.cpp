#include "StrategyFactory.h"

#include "strategies/FirstComeFirstServed.h"
#include "strategies/PriorityRoundRobin.h"
#include "strategies/ShortestJobFirst.h"

// remake to use std::unordered map
//

const std::unordered_map<std::string, std::function<std::unique_ptr<SchedulerStrategy>()>>
    strategy_map = {{"rr", []() { return std::make_unique<PriorityRoundRobin>(); }},
                    {"fcfs", []() { return std::make_unique<FirstComeFirstServed>(); }},
                    {"sjf", []() { return std::make_unique<ShortestJobFirst>(); }}};

std::unique_ptr<SchedulerStrategy> StrategyFactory::createStrategy_map(const std::string& algo_name)
{
    auto mapIter = strategy_map.find(algo_name);
    if (mapIter != strategy_map.end())
    {
        return mapIter->second();
    }
    return std::make_unique<PriorityRoundRobin>();
}

std::unique_ptr<SchedulerStrategy> StrategyFactory::createStrategy(const std::string& algo_name)
{
    if (algo_name == "rr" || algo_name == "RR")
    {
        return std::make_unique<PriorityRoundRobin>();
    }
    else if (algo_name == "fcfs" || algo_name == "FCFS")
    {
        return std::make_unique<FirstComeFirstServed>();
    }
    else if (algo_name == "sjf" || algo_name == "SJF")
    {
        return std::make_unique<ShortestJobFirst>();
    }
    else
    {
        return std::make_unique<PriorityRoundRobin>();  // Default to Round-Robin if no algo
                                                        // provided
    }
}
