#include "Metrics.h"

#include <LogsJson.h>

Metrics::Metrics(std::array<PCB, N>& process_pool) : process_pool(process_pool), metrics_calculated(false)
{
}

SystemMetrics Metrics::calculate(int total_time)
{
    SystemMetrics metrics;
    // for each process loop through and get the needed characteristics for the
    // system metrics. add that to the 'metrics' and return that when finished.
    // set cached_metrics to the metrics before returning the metrics.

    metrics.total_time = total_time;
    metrics.total_processes = N;

    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    int total_cpu_time = 0;

    for (const auto& proc : process_pool)
    {
        ProcessMetrics pm = calculateProcessMetrics(proc);
        total_turnaround_time += pm.turnaround_time;
        total_waiting_time += pm.waiting_time;
        total_response_time += pm.response_time;
        total_cpu_time += pm.cpu_time_used;

        metrics.per_process.push_back(pm);
    }

    metrics.avg_turnaround_time = static_cast<double>(total_turnaround_time) / N;
    metrics.avg_waiting_time = static_cast<double>(total_waiting_time) / N;
    metrics.avg_response_time = static_cast<double>(total_response_time) / N;
    metrics.cpu_utilization = (static_cast<double>(total_cpu_time) / total_time) * 100;
    metrics.throughput = static_cast<double>(N) / total_time;

    cached_metrics = metrics;
    metrics_calculated = true;

    return metrics;
}

ProcessMetrics Metrics::calculateProcessMetrics(const PCB& proc) const
{
    // get all the needed metrics from the process do the necessary caluclation
    // here so they are plug and play in system metrics
    ProcessMetrics pm{};

    // get process pid
    pm.pid = proc.getPid();

    pm.completion_time = proc.getCompletionTime();

    // all processes arrive at 0.
    pm.turnaround_time = proc.getCompletionTime() - 0;

    pm.cpu_time_used = proc.getTotalCpuUsedTime();

    pm.io_time_used = proc.getTotalIOTime();

    pm.waiting_time = pm.turnaround_time - pm.cpu_time_used - pm.io_time_used;

    pm.response_time = proc.getFirstResponseTime();

    return pm;
}

json Metrics::toJson() const
{
    if (!metrics_calculated)
    {
        throw std::runtime_error("Metrics not calculated yet");
    }

    json summary;
    summary["system_metrics"] = {{"Average turnaround time", cached_metrics.avg_turnaround_time},
                                 {"Average waiting time", cached_metrics.avg_waiting_time},
                                 {"Average response time", cached_metrics.avg_response_time},
                                 {"CPU Utilization", cached_metrics.cpu_utilization},
                                 {"Throughput", cached_metrics.throughput},
                                 {"total time", cached_metrics.total_time},
                                 {"total processes", cached_metrics.total_processes}},
    summary["process_metrics"] = json::array();

    for (const auto& pm : cached_metrics.per_process)
    {
        summary["process_metrics"].push_back({{"PID", pm.pid},
                                              {"Turnaround time", pm.turnaround_time},
                                              {"Waiting time", pm.waiting_time},
                                              {"Response time", pm.response_time},
                                              {"CPU time used", pm.cpu_time_used},
                                              {"IO time used", pm.io_time_used},
                                              {"Completion time", pm.completion_time}});
    }
    return summary;
}
void Metrics::writeToFile(const std::string& name) const
{
    json summary = toJson();
    createJSON(name);
    appendToJSON(name, summary);
}

// return specific process metrics from pid
ProcessMetrics Metrics::getProcessMetrics(int pid) const
{
    if (!metrics_calculated)
    {
        throw std::runtime_error("Metrics not calculated yet");
    }
    for (const auto& proc : cached_metrics.per_process)
    {
        if (proc.pid == pid)
        {
            return proc;
        }
    }
    throw std::runtime_error("Process not found");
}
double Metrics::getAvgTurnaroundTime() const
{
    if (!metrics_calculated)
    {
        throw std::runtime_error("Metrics not calculated yet");
    }
    return cached_metrics.avg_turnaround_time;
}
double Metrics::getCpuUtilization() const
{
    if (!metrics_calculated)
    {
        throw std::runtime_error("Metrics not calculated yet");
    }
    return cached_metrics.cpu_utilization;
}
