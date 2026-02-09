#include "SchedulerClass.h"

#include <sstream>

#include "IOManager.h"
#include "LogsJson.h"
#include "PCB.h"
#include "ReadyQueue.h"

Scheduler::Scheduler(std::string logs_name) : IO_Processes(process_pool), metrics(process_pool), logs_name(logs_name)
{
    createJSON(logs_name);
}

// time spent in ready queue
void Scheduler::findWaitTime()
{
}

// total time from submission to completion
void Scheduler::findTurnaroundTime()
{
    //
}

// time from submission to first reponse
void Scheduler::findResponseTime()
{
    //
}

void Scheduler::priorityScheduling()
{
    std::sort(process_pool.begin(), process_pool.end(), [](const PCB& a, const PCB& b) { return a.getPriority() < b.getPriority(); });

    debug(EXEC, "Process order");
    for (const auto& proc : process_pool)
    {
        debug(EXEC, "PID: " + std::to_string(proc.getPid()) + ", burst time: " + std::to_string(proc.getBurstTime()) + ", priority: " + std::to_string(proc.getPriority()));
    }
}

void Scheduler::updateQueuesAfterAging(PCB* p, int& time_slice)
{
    //***** Update aging *****//
    // phase 1 - find aged processes, and store them in vector
    std::vector<size_t> aged_processes;
    for (size_t idx = 0; idx < process_pool.size(); idx++)
    {
        if (process_pool[idx].getRemainingTime() > 0 && process_pool[idx].getPid() != p->getPid() && !process_pool[idx].isWaitingIO() && readyQueue[process_pool[idx].getPriority()].contains(idx))
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
        appendToJSON(logs_name, e);
    }
}

bool Scheduler::cleanUpQueues(int& currentTime, int& lastTime)
{
    // Check if all work is done.
    bool hasRemainingWork = std::any_of(process_pool.begin(), process_pool.end(), [](const PCB& p) { return p.getRemainingTime() > 0; });

    // If this condition is met, all processes done.
    if (!hasRemainingWork && IO_Processes.isEmpty())
    {
        lastTime = currentTime;
        return false;
    }

    // Check if Ready Queue has processes to process.
    bool anyQueueHasWork = std::any_of(readyQueue.begin() + 1, readyQueue.end(), [](const auto& q) { return !q.empty(); });

    // If all processes are in IO wait, advance time
    if (!anyQueueHasWork && !IO_Processes.isEmpty())
    {
        // Find minimum IO remaining time
        int minTime = IO_Processes.getMinRemainingIOTime();

        debug(QUEUE, "All processes in IO wait, advancing time by " + std::to_string(minTime) + "ms");
        currentTime += minTime;

        // Process IO completions
        IO_Processes.processIO(minTime);

        // Hold finished processes, and reinsert back to ready queue.
        const auto& finished = IO_Processes.getFinishedProcesses();
        for (auto idx : finished)
        {
            PCB& proc = process_pool[idx];
            if (proc.getRemainingTime() > 0)
            {
                debug(QUEUE, "[IO DONE] PID " + std::to_string(proc.getPid()) + " resuming from IO at time " + std::to_string(currentTime));
                readyQueue[proc.getPriority()].push(idx);
            }
        }

        if (!finished.empty())
        {
            IO_Processes.clearFinished();
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
        if (process_pool[p].getPriority() < 0 || process_pool[p].getPriority() > MAX_PRIORITY)  // use clamp!
        {
            process_pool[p].setPriority(std::clamp(process_pool[p].getPriority(), 1, MAX_PRIORITY));
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
        for (auto el = 1; el <= MAX_PRIORITY; ++el)
        {
            if (!readyQueue[el].empty())
            {
                debug(QUEUE,
                      [&]()
                      {
                          std::ostringstream oss;
                          oss << "=== Queue status at time " << currentTime << " ===\n";
                          for (int prio = 1; prio <= MAX_PRIORITY; ++prio)
                          {
                              if (!readyQueue[prio].empty())
                              {
                                  oss << "Priority " << prio << ": " << readyQueue[prio] << "\n";
                              }
                              oss << "IO wait queue size: " << IO_Processes.size() << "\n========================\n";
                          }
                          return oss.str();
                      });

                size_t i = readyQueue[el].pop();
                PCB& p = process_pool[i];

                //***** calculate time delta *****//
                int delta = currentTime - lastTime;

                //***** IO Wait Queue management *****//
                // want to process IO first, for alredy waiting processes
                if (delta > 0)
                {
                    IO_Processes.processIO(delta);

                    // move finished IO processes
                    const auto& finished = IO_Processes.getFinishedProcesses();
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
                        IO_Processes.clearFinished();
                    }
                }

                //***** To track first response for processes *****//
                if (p.isFirstResponse() == false)  // if its the process' first time about to execute, set these values.
                {
                    p.recordFirstResponse(currentTime);
                }

                //***** Execute process *****//
                int timeElapsed = p.execute(TIME_QUANTUM);
                currentTime += timeElapsed;

                debug(EXEC,
                      [&]()
                      {
                          std::ostringstream oss;
                          oss << "[EXEC] PID " << p.getPid() << " ran for " << timeElapsed << " -> remaining: " << p.getRemainingTime() << " at time " << currentTime;
                          return oss.str();
                      });

                //***** handle state transitions + Update IO Wait Queue *****//

                if (p.isWaitingIO() && !IO_Processes.containsPID(p.getPid()))
                {
                    IO_Processes.enqueue(i);
                    IO_Processes.updateIO();
                }

                if (p.isReady() && timeElapsed > 0)
                    readyQueue[el].push(i);

                if (p.getRemainingTime() <= 0)
                {
                    p.setCompletionTime(currentTime);
                }

                //***** Update aging *****//
                updateQueuesAfterAging(&p, delta);

                lastTime = currentTime;
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
                // debug(EXEC, "In Process_State ready");
                curr_state = Process_STATE::RUNNING;
                break;

            case Process_STATE::RUNNING:
                // debug(EXEC, "In Process_State Running");
                priorityScheduling();
                roundRobin();
                curr_state = Process_STATE::FINISHED;
                break;

            case Process_STATE::FINISHED:
                // debug(EXEC, "In Process_State finished");
                SystemMetrics sm{};
                sm = metrics.calculate(currentTime);
                metrics.writeToFile(logs_name + "_metrics");
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

void Scheduler::simulateTime(int ms)
{
    // std::this_thread::sleep_for(std::chrono::milliseconds(ms));
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
