#pragma once
#include "SchedulerStrategy.h"

class ShortestJobFirst : public SchedulerStrategy
{
   public:
    std::optional<size_t> selectNext(std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>>& queues,
                                     const std::vector<PCB>& process_pool,
                                     int currentTime) override
    {
        // store the provisional process with the shortest remaining time while iterating through
        // all processes in that priority.

        size_t shortest_idx{0};
        int shortest_time = INT_MAX;
        bool found = false;

        for (int prio = 1; prio <= queues.size() - 1; ++prio)
        {
            if (!queues[prio].empty())
            {
                size_t temp_idx = queues[prio].front();
                std::cout << "Choose from FCFS: " << queues[prio].front() << std::endl;

                if (process_pool[temp_idx].getRemainingTime() < shortest_time)
                {
                    shortest_time = process_pool[temp_idx].getRemainingTime();
                    shortest_idx = temp_idx;
                    found = true;
                }
            }
            if (found)
            {
                for (auto& q : queues)
                {
                    if (q.remove(shortest_idx))
                        break;
                }
                return shortest_idx;
            }
        }
        return std::nullopt;  // absence of value -> no process to pop
    }

    std::string name() const override
    {
        return "Shortest Job First Strategy";
    }
};