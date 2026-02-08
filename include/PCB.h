#pragma once
#include <ostream>

constexpr int TIME_QUANTUM = 4;
constexpr int AGING_THRESHOLD = 5;
constexpr int N = 6;

enum class ProcessState {
  READY = 0,
  RUNNING = 1,
  WAITING_IO = 2,
  FINISHED = 3
};

class PCB {
  friend std::ostream &operator<<(std::ostream &out, const PCB &p);

public:
  PCB(int pid, int prio, int burst, bool io_bound, int io_interval);

  int execute(int timeslice);
  int computeExecuteTime(int timeslice);
  bool ageProcess(int timediff);
  void updateStatesAfterExecution();

  // Getters
  int getPid() const;
  int getPriority() const;
  int getOldPriority() const;
  int getBurstTime() const;
  int getCpuUsed() const;
  int getIOInterval() const;
  int getRemainingTime() const;
  int getIORemainingTime() const;
  int getWaitingTime() const;
  int getCompletionTime() const;
  int getTotalIOTime() const;
  bool isIOBound() const;

  ProcessState getState() const;
  std::string getStringState() const;

  // Setters
  void setState(ProcessState state);
  void setPriority(int prio);
  void setIOTime(int io_remainingtime);
  void resetIO();
  void startIO();

  // Boolean helpers for state
  bool isReady() const;
  bool isRunning() const;
  bool isWaitingIO() const;
  bool isFinished() const;

private:
  int pid;             // process id
  int prio;            // current priority
  int old_prio;        // original prio
  int bursttime;       // burst time
  int remainingtime;   // remaining time after burst
  int waiting_time;    // time a process has waited to be executed
  bool io_bound;       // true if process is I/O bound
  int io_interval;     // how often in ms it performs I/O
  int io_remaining;    // how long left of io operation
  int cpu_used;        // cumulative CPU time used since last I/O
  int completion_time; // set this, when process completes
  int total_io_time;   // total time spent in i/o wait
  ProcessState PS;     // Process State
};