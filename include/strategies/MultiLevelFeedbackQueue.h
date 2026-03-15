// Future Work implementing MLFQ

/*
RULES :
- Higher Priority runs first
    Q1 > Q2 > Q3 ...

- Same Queues use RR

- New Processes start at the top (highest priority queue)

- A process using its entire time quantum -> demote to lower queue
    -> Means they are CPU-bound, while we want interactive programs at the top

- A process that yields keeps its priority

- Aging / Starvation boost
    -> Every T time all processes boosted to top queue

- Uses Multiple time quantums
     -> Q1 = 2 - Q2 = 4 - Q3 = 8 ...
*/

#pragma once
#include "SchedulerStrategy.h"

class MultiLevelFeedbackQueue : public SchedulerStrategy
{
   private:
    // add 0 as index 0, so its aligned with the processes priorities.
    std::vector<int> time_quantums = {0, 2, 4, 8};

   public:
    std::optional<size_t> selectNext(std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>>& queues,
                                     const std::vector<PCB>& process_pool,
                                     int currentTime) override
    {
        for (auto prio = 1; prio <= queues.size() - 1; ++prio)
        {
            if (!queues[prio].empty())
            {
                return queues[prio].pop();
            }
        }
        return std::nullopt;  // absence of value -> no process to pop
    }

    // return time quantum
    int getTimeSlice(const PCB& process, int default_quantum) const override
    {
        int prio = process.getPriority();
        if (prio >= time_quantums.size())
        {
            return default_quantum;
        }
        return time_quantums[prio];
    }

    // only preempt if the new process is of higher priority
    bool shouldPreempt(const PCB& current_process, const PCB& new_process) const override
    {
        return current_process.getPriority() < new_process.getPriority();
    }

    // rr reinsert at priority level
    int getReinsertionPolicy(const PCB& process, int time_elapsed) const override
    {
        if (time_elapsed == time_quantums[process.getPriority()])
        {
            if (process.getPriority() !=
                time_quantums.size())  // check it is not in the lowest priority queue
            {
                return process.getPriority() + 1;
            }
            else
            {
                return process.getPriority();  // already at the lowest possible queue
            }
        }
        return process.getPriority();
    }

    // uses priority
    bool usesPriorityQueues() const override
    {
        return true;
    }

    // rr uses aging
    bool usesAging() const override
    {
        return true;
    }

    std::string name() const override
    {
        return "Multi Level Feedback Queue Strategy";
    }

    std::string description() const override
    {
        return "Multi-level Feedback Queue, for maximal interactive programs";
    }
};
