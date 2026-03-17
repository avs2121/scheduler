#pragma once
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "ConfigLoader.h"
#include "Constants.h"
#include "IOManager.h"
#include "LogsJson.h"
#include "Metrics.h"
#include "PCB.h"
#include "ReadyQueue.h"
#include "StrategyFactory.h"

class Scheduler
{
   public:
    Scheduler(std::string logs_name, std::string config_file);

    // Debug logger
    enum DebugLevel
    {
        NONE = 0,          // 0
        EXEC = 1 << 0,     // 1
        AGING = 1 << 1,    // 2
        IO = 1 << 2,       // 4
        QUEUE = 1 << 3,    // 8
        WARNING = 1 << 4,  // 16
        EXTRA = 1 << 5,    // 32
        ALL = 0xFFFF
    };

    // Debug
    void setDebugFlags(int flags);
    void enableDebug(DebugLevel level);
    void disableDebug(DebugLevel level);
    bool isDebugEnabled(DebugLevel level) const;
    int getDebugLevel() const;

    // Statemachine
    void run();

    ~Scheduler() = default;

   private:
    // Logging
    void logEvent(PCB* p);
    void flushLogs();

    // Scheduling + Queues setup.
    void QueueSorting();
    bool cleanUpQueues(int& currentTime, int& lastTime);
    void runSchedulingLoop();

    // load config
    void loadConfig(std::string config_file);

    // Queue handler
    void PopulateInitialQueues();  // Queue initialization
    int getTargetQueues(PCB& proc);
    void loadArrivingProcesses(int currentTime);  // dynamic arrivals
    void initializeUnscheduled();                 // keep unscheduled processes structured

    void updateQueuesAfterAging(PCB* p, int& time_slice);
    void processIOCompletions(int& delta);
    void handleProcessStateTransistion(PCB& p, size_t idx);

    // Helper functions to debug
    // Debug helper for lambdas
    template <typename Func>
    auto debug(DebugLevel category, Func&& func) const -> decltype(func(), void())
    {
        if (debug_level & category)
        {
            std::cout << func() << std::endl;
        }
    }

    // Debug helper for direct values
    template <typename T>
    auto debug(DebugLevel category, T&& msg) const -> decltype(std::cout << msg, void())
    {
        if (debug_level & category)
        {
            std::cout << std::forward<T>(msg) << std::endl;
        }
    }

    // Default debug-level set to NONE
    int debug_level = NONE;

    // Helper methods
    PCB& getProcessByPID(int pid);
    size_t pidToIndex(int pid) const;
    int indexToPid(size_t idx) const;

    // hold the unscheduled processes
    std::vector<size_t> unscheduled_processes;

    // hold the loaded processes
    std::vector<PCB> process_pool;

    int currentTime;
    std::optional<PCB> lastProcess;

    std::unique_ptr<SchedulerStrategy> strategy;

    std::vector<ReadyQueue<size_t, MAX_PROCESS_SIZE>> readyQueue;
    std::optional<IOManager> IO_Processes;  // for lazy/delayed initialization
    std::optional<Metrics> metrics;         // for lazy/delayed initialization

    std::string logs_name;
    std::vector<json> eventLog;  // store json objects for logging

    std::string config_file;
    ConfigLoader loader;
    SchedulerConfig sched_conf;
    std::vector<ProcessConfig> proc_conf;

    int time_quantum_sched;
    int aging_threshold_sched;
    int max_priority_sched;
    int context_switch_time_sched;
    std::string algorithm_sched;
};
