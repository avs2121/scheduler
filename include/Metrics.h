#pragma once
#include <array>
#include <vector>

#include "PCB.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// right now turnaround and completion is the same, because they all arrive at (0) - same time.
struct ProcessMetrics
{
    int pid;              // process id
    int turnaround_time;  // total time : completion time - arrival time
    int waiting_time;     // time in i/o wait : turnaround time - cpu time - io time
    int response_time;    // time from arrival to first execution : response time - arrival time
    int cpu_time_used;    // cpu time used on process
    int io_time_used;     // time spent in i/o
    int completion_time;  // time worked on : time from arrival
};

struct SystemMetrics
{
    double avg_turnaround_time;
    double avg_waiting_time;
    double avg_response_time;
    double cpu_utilization;  // (total cpu time / total time) * 100 (for procent)
    double throughput;       // total processes / total time
    int total_time;          // total time spent on all processes
    int total_processes;     // amount of processes
    std::vector<ProcessMetrics> per_process;
};

class Metrics
{
   public:
    Metrics(std::vector<PCB>& process_pool);

    // calculate metrics from process pool
    SystemMetrics calculate(int total_time);

    // to json format
    // write to file
    json toJson() const;
    void writeToFile(const std::string& name) const;

    // process metrics
    ProcessMetrics getProcessMetrics(int pid) const;
    double getAvgTurnaroundTime() const;
    double getCpuUtilization() const;

   private:
    const std::vector<PCB>& process_pool;
    SystemMetrics cached_metrics;
    bool metrics_calculated;

    // private helper methods
    ProcessMetrics calculateProcessMetrics(const PCB& proc) const;
};