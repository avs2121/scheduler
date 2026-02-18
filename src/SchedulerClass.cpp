#include "SchedulerClass.h"

#include <format>
#include <ranges>
#include <sstream>

#include "IOManager.h"
#include "LogsJson.h"
#include "PCB.h"
#include "ReadyQueue.h"

Scheduler::Scheduler(std::string logs_name, std::string config_file)
    : logs_name(logs_name), config_file(config_file), loader(config_file)
{
    loadConfig(config_file);
    this->logs_name = std::filesystem::path(logs_name).stem().string();
    createJSON(logs_name);
}

void Scheduler::loadConfig(std::string config_file)
{
    sched_conf = loader.getSchedulerConfig();
    time_quantum_sched = sched_conf.time_quantum;
    aging_threshold_sched = sched_conf.aging_threshold;
    max_priority_sched = sched_conf.max_priority;
    context_switch_time_sched = sched_conf.context_switch_time;

    readyQueue.resize(max_priority_sched +
                      1);  // after getting the max_priority value, resize the container.

    proc_conf = loader.getProcessConfig();
    if (proc_conf.size() > MAX_PROCESS_SIZE)
    {
        throw std::runtime_error("To many processes, the maximum size is: " +
                                 std::to_string(MAX_PROCESS_SIZE));
    }

    process_pool.clear();
    process_pool.reserve(proc_conf.size());  // optional, but avoids re-allocs.

    for (const auto& pc : proc_conf)
    {
        process_pool.emplace_back(pc.pid,
                                  pc.priority,
                                  pc.burst,
                                  pc.io_bound,
                                  pc.io_interval,
                                  aging_threshold_sched,
                                  time_quantum_sched);
    }

    // lazy initialization with std::optional,
    // used because the values loading, isnt known before this point.
    IO_Processes.emplace(process_pool);
    metrics.emplace(process_pool);
}

void Scheduler::priorityScheduling()
{
    // sort the process pool, by lowest priority first, by using ranges with projections
    std::ranges::sort(process_pool, std::ranges::less{}, &PCB::getPriority);

    debug(EXEC, "Process order");
    for (const auto& proc : process_pool)
    {
        debug(EXEC,
              std::format("PID: {}, Burst Time: {}, Priority: {}",
                          proc.getPid(),
                          proc.getBurstTime(),
                          proc.getPriority()));
    }
    debug(EXEC,
          std::format(
              "Time Quantum: {}, Max Priority: {}, Aging Threshold: {}, Context Switch Time: {}",
              time_quantum_sched,
              max_priority_sched,
              aging_threshold_sched,
              context_switch_time_sched));
}

void Scheduler::updateQueuesAfterAging(PCB* p, int& time_slice)
{
    //***** Update aging *****//
    // phase 1 - find aged processes, and store them in vector
    /* clang-format off
    std::vector<size_t> aged_processes;
    for (size_t idx = 0; idx < process_pool.size(); idx++)
    {
        if (process_pool[idx].getRemainingTime() > 0 && 
            process_pool[idx].getPid() != p->getPid() && 
            !process_pool[idx].isWaitingIO() && 
            readyQueue[process_pool[idx].getPriority()].contains(idx))
        {
            if (process_pool[idx].ageProcess(time_slice))
            {
                aged_processes.push_back(idx);
            }
        }
    }

    // phase 2 - remove and push the aged processes, to appropriate queues.
    for (auto id : aged_processes)
    {
        readyQueue[process_pool[id].getOldPriority()].remove(id);
        readyQueue[process_pool[id].getPriority()].push(id);
    }
    */

    // clang-format off
    
    /*
    The above style is the clearly favored style, because it is way easier to understand. There is a lof of nested conditionals, 
    and modifications during the loops, therefore ranges and views, should probably not be prefered. 
    It is though, because i wanted to practice them. Therefore the above, out-commented code is kept. 
    */

    std::vector<size_t> aged_processes;
    
    //  First use std::views with iota, for iterating over all the processes in the pool, and do the necessary checks. 
    auto idx_range = std::views::iota((size_t)0, process_pool.size())  |  
    std::views::filter([&](size_t idx){PCB& proc = process_pool[idx]; 
        return proc.getRemainingTime() > 0 && 
        (proc.getPid() != p->getPid()) && 
        !proc.isWaitingIO() && 
        readyQueue[proc.getPriority()].contains(idx); });

    // For the processes ful-filling the previous conditionals, use ranges to iterate over each and age. 
    std::ranges::for_each(idx_range, ([&] (size_t idx) {
        if(process_pool[idx].ageProcess(time_slice))
            aged_processes.push_back(idx);}));

    // Use ranges on each element that fulfilled previous conditionals, to update their priority. 
    std::ranges::for_each(aged_processes, [&](size_t idx) {
        readyQueue[process_pool[idx].getOldPriority()].remove(idx);
        readyQueue[process_pool[idx].getPriority()].push(idx);
    });

    // clang-format on
}

void Scheduler::logEvent(PCB* p)
{
    enum class Event
    {
        RUNNING,
        IO_WAIT,
        FINISHED
    };

    Event event = Event::RUNNING;
    if (p->getRemainingTime() == 0)
        event = Event::FINISHED;
    else if (p->isWaitingIO())
        event = Event::IO_WAIT;
    else
        event = Event::RUNNING;

    json j_array = {{"pid", p->getPid()},
                    {"prio", p->getPriority()},
                    {"event", event},
                    {"waiting time", p->getWaitingTime()},
                    {"remaining time", p->getRemainingTime()},
                    {"io bound", p->isIOBound()},
                    {"io interval", p->getIOInterval()},
                    {"CPU used", p->getCpuUsed()},
                    {"Process State", p->getStringState()}};

    eventLog.push_back(j_array);
}

void Scheduler::flushLogs()
{
    for (auto& e : eventLog)
    {
        appendToJSON_array(logs_name, e);
    }
}

bool Scheduler::cleanUpQueues(int& currentTime, int& lastTime)
{
    // Check if all work is done.
    bool hasRemainingWork =
        std::ranges::any_of(process_pool, [](const PCB& p) { return p.getRemainingTime() > 0; });

    // If this condition is met, all processes done.
    if (!hasRemainingWork && IO_Processes->isEmpty())
    {
        lastTime = currentTime;
        return false;
    }

    // Check if Ready Queue has processes to process. Remember to skip first element (priority = 0).
    bool anyQueueHasWork = std::ranges::any_of(readyQueue | std::views::drop(1),
                                               [](const auto& q) { return !q.empty(); });

    // If all processes are in IO wait, advance time
    if (!anyQueueHasWork && !IO_Processes->isEmpty())
    {
        // Find minimum IO remaining time
        int minTime = IO_Processes->getMinRemainingIOTime();
        debug(QUEUE, std::format("All processes in IO wait, advancing time by {}", minTime));
        currentTime += minTime;

        // Process IO completions
        IO_Processes->processIO(minTime);

        // Hold finished processes, and reinsert back to ready queue.
        const auto& finished = IO_Processes->getFinishedProcesses();
        for (auto idx : finished)
        {
            PCB& proc = process_pool[idx];
            if (proc.getRemainingTime() > 0)
            {
                debug(QUEUE,
                      std::format("[IO DONE] PID: {}, resuming from IO at total time {}",
                                  proc.getPid(),
                                  currentTime));
                readyQueue[proc.getPriority()].push(idx);
            }
        }

        if (!finished.empty())
        {
            IO_Processes->clearFinished();
        }

        lastTime = currentTime;

        return true;
    }
    return true;
}

void Scheduler::roundRobin()
{
    for (size_t p = 0; p < process_pool.size(); p++)
    {
        if (process_pool[p].getPriority() < 0 ||
            process_pool[p].getPriority() > max_priority_sched)  // use clamp!
        {
            process_pool[p].setPriority(
                std::clamp(process_pool[p].getPriority(), 1, max_priority_sched));
            debug(WARNING, "Clamping prio - otherwise out of index");
        }
        readyQueue[process_pool[p].getPriority()].push(p);
    }

    currentTime = 0;  // track current time
    int lastTime = 0;

    while (true)
    {
        if (!cleanUpQueues(currentTime, lastTime))
        {
            break;
        }
        // Execute process
        for (auto el = 1; el <= max_priority_sched; ++el)
        {
            if (!readyQueue[el].empty())
            {
                debug(QUEUE,
                      [&]()
                      {
                          std::ostringstream oss;
                          // Ready queues

                          for (int prio = 1; prio <= max_priority_sched; ++prio)
                          {
                              oss << "Priority: " << prio << " contains: ";
                              for (size_t idx : readyQueue[prio].toVector())
                              {
                                  oss << "PID: " << process_pool[idx].getPid() << " ";
                              }
                              oss << "\n";
                          }
                          oss << "\n========================\n";
                          // IO queue
                          oss << "IO wait queue: ";

                          for (size_t idx : IO_Processes->getQueue())
                          {
                              oss << "PID: " << process_pool[idx].getPid() << " ";
                          }

                          oss << "\n========================\n";
                          oss << "IO wait queue size: " << IO_Processes->size();

                          return oss.str();
                      });

                size_t i = readyQueue[el].pop();

                PCB& p = process_pool[i];

                //***** check if context switch *****//
                if (lastProcess.has_value() && p.getPid() != lastProcess->getPid())
                {
                    currentTime += context_switch_time_sched;  // increment with context switch
                }

                //***** calculate time delta *****//
                int delta = currentTime - lastTime;

                //***** IO Wait Queue management *****//
                // want to process IO first, for alredy waiting processes
                if (delta > 0)
                {
                    IO_Processes->processIO(delta);

                    // move finished IO processes
                    const auto& finished = IO_Processes->getFinishedProcesses();
                    for (size_t idx : finished)
                    {
                        PCB& proc = process_pool[idx];
                        if (proc.getRemainingTime() > 0 && proc.isReady())
                        {
                            readyQueue[proc.getPriority()].push(idx);
                        }
                    }
                    if (!finished.empty())
                    {
                        IO_Processes->clearFinished();
                    }
                }

                //***** To track first response for processes *****//
                if (!p.isFirstResponse())  // if its the process' first time about to execute, set
                                           // these values.
                {
                    p.recordFirstResponse(currentTime);
                }

                //***** Execute process *****//
                int timeElapsed = p.execute(time_quantum_sched);
                currentTime += timeElapsed;
                debug(EXEC,
                      std::format("[EXEC] PID: {} ran for {} -> remaining time: {} at time {}",
                                  p.getPid(),
                                  timeElapsed,
                                  p.getRemainingTime(),
                                  currentTime));

                //***** handle state transitions + Update IO Wait Queue *****//

                if (p.isWaitingIO() && !IO_Processes->containsPID(p.getPid()))
                {
                    IO_Processes->enqueue(i);
                    IO_Processes->updateIO();
                }

                if (p.isReady() && timeElapsed > 0)
                    readyQueue[el].push(i);

                if (p.getRemainingTime() <= 0)
                {
                    debug(EXEC, std::format("Process PID: {}, finished", p.getPid()));
                    p.setCompletionTime(currentTime);
                }

                //***** Update aging *****//
                updateQueuesAfterAging(&p, delta);

                lastTime = currentTime;
                lastProcess.emplace(p);
                //***** Update logs *****//
                logEvent(&p);
                break;
            }
        }
    }

    // when finished write all to logs.
    debug(EXEC, "Flushing logs");
    flushLogs();
}

void Scheduler::run()
{
    enum Process_STATE
    {
        READY,
        RUNNING,
        FINISHED,
    };
    Process_STATE curr_state = Process_STATE::READY;

    bool finished_flag = false;
    while (!finished_flag)
    {
        switch (curr_state)
        {
            case Process_STATE::READY:
                debug(EXEC, "In Process_State ready");
                priorityScheduling();
                curr_state = Process_STATE::RUNNING;
                break;

            case Process_STATE::RUNNING:
                debug(EXEC, "In Process_State Running");
                roundRobin();
                curr_state = Process_STATE::FINISHED;
                break;

            case Process_STATE::FINISHED:
                debug(EXEC, "In Process_State finished");
                SystemMetrics sm{};
                sm = metrics->calculate(currentTime);
                metrics->writeToFile(logs_name + "_metrics");
                finished_flag = true;
                break;
        }
    }
}

PCB& Scheduler::getProcessByPID(int pid)
{
    return process_pool[pid - 1];
}

size_t Scheduler::pidToIndex(int pid) const
{
    return pid - 1;
}

int Scheduler::indexToPid(size_t idx) const
{
    return idx + 1;
}

void Scheduler::setDebugFlags(int flags)
{
    this->debug_level = flags;
}

void Scheduler::enableDebug(DebugLevel category)
{
    debug_level |= category;  // Logical OR
}

void Scheduler::disableDebug(DebugLevel category)
{
    debug_level &= ~category;  // Logical AND with one's complement (bitwise NOT)
}

bool Scheduler::isDebugEnabled(DebugLevel category) const
{
    return ((debug_level & category) != 0);
}

int Scheduler::getDebugLevel() const
{
    return debug_level;
}
