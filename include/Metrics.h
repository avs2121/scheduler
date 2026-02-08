#pragma once
#include "PCB.h"
#include "nlohmann/json.hpp"
#include <array>
#include <vector>

using json = nlohmann::json;

// clang-format off
// right now turnaround and completion is the same, because they all arrive at (0) - same time.
struct ProcessMetrics {
  int pid;             // process id
  int turnaround_time; // total time : completion time - arrival time
  int waiting_time;  // time in i/o wait : turnaround time - cpu time - io time
  int response_time; // time from arrival to first execution -> add method to pcb to track this. 
  int cpu_time_used; // cpu time used on process
  int io_time_used;  // time spent on i/o operation
  int completion_time; // time worked on : time from arrival
};
// clang-format on 

struct SystemMetrics {
  double avg_trunaround_time;
  double avg_waiting_time;
  double avg_response_time;
  double cpu_utilization; // (total cpu time / total time) * 100 (for procent)
  double throughput;      // total processes / total time
  int total_time;         // total time spent on all processes
  int total_processes;    // amount of processes
  std::vector<ProcessMetrics> per_process;
};

class Metrics {
public:
    Metrics(std::array<PCB, N * 2>& process_pool);

    //calculate metrics

    //to json format

    //write to file 

private:
};