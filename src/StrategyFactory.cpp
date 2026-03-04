#include "StrategyFactory.h"

#include <string.h>

#include "FirstComeFirstServed.h"
#include "PriorityRoundRobin.h"
#include "ShortestJobFirst.h"

std::unique_ptr<SchedulerStrategy> StrategyFactory::createStrategy(const std::string& algo_name)
{
    if (strcmp("rr", algo_name.c_str()) == 0 || strcmp("RR", algo_name.c_str()) == 0)
    {
        return std::make_unique<PriorityRoundRobin>();
    }
    else if (strcmp("fcfs", algo_name.c_str()) == 0 || strcmp("FCFS", algo_name.c_str()) == 0)
    {
        return std::make_unique<FirstComeFirstServed>();
    }
    else if (strcmp("sjf", algo_name.c_str()) == 0 || strcmp("SJF", algo_name.c_str()) == 0)
    {
        return std::make_unique<ShortestJobFirst>();
    }
    else
    {
        return std::make_unique<PriorityRoundRobin>();  // Default to Round-Robin if no algo
                                                        // provided
    }
}
