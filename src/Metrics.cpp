#include "Metrics.h"

Metrics::Metrics(std::array<PCB, N * 2> &process_pool)
    : process_pool(process_pool), metrics_calculated(false) {}

SystemMetrics Metrics::calculate(int total_time) {
  SystemMetrics metrics;
  // for each process loop through and get the needed characteristics for the
  // system metrics. add that to the 'metrics' and return that when finished.
  // set cached_metrics to the metrics before returning the metrics.
}

ProcessMetrics Metrics::calculateProcessMetrics(const PCB &proc) const {
  // get all the needed metrics from the process do the necessary caluclation
  // here so they are plug and play in system metrics
}

json Metrics::toJson() const {
  // format the data to json format.
  // one for system metrics and one for each process
}
void Metrics::writeToFile(const std::string &name) const {
  // make use of plug and play commands from logs (create file and append)
}

ProcessMetrics Metrics::getProcessMetrics(int pid) const {
  // return specific process metrics from pid
}
double Metrics::getAvgTurnaroundTime() const {
  // return avg turnaround time
}
double Metrics::getCpuUtilization() const {
  // return cpu utilization
}

double Metrics::calculateCpuUtilization(int total_cpu_time,
                                        int total_time) const {
  // calculate cpu utilization here
}
double Metrics::calculateThroughput(int total_time, int num_processes) const {
  // calculate throughput here
}
