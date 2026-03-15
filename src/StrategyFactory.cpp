#include "StrategyFactory.h"

#include <algorithm>

std::unique_ptr<SchedulerStrategy> StrategyFactory::createStrategy_map(std::string& algo_name)
{
    // transforming to lower-case letters to adhere to lambda function calls for each strategy
    std::transform(algo_name.begin(),
                   algo_name.end(),
                   algo_name.begin(),
                   [](unsigned char c) { return std::tolower(c); });

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
    else if (algo_name == "mlfq" || algo_name == "MLFQ")
    {
        return std::make_unique<MultiLevelFeedbackQueue>();
    }
    else
    {
        return std::make_unique<PriorityRoundRobin>();  // Default to Round-Robin if no algo
                                                        // provided
    }
}
