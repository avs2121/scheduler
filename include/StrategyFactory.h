#pragma once
#include <memory>

#include "SchedulerStrategy.h"


class StrategyFactory
{
    virtual std::unique_ptr<SchedulerStrategy> createStrategy() = 0;
    virtual ~StrategyFactory() = default;
};