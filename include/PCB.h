#pragma once
#include <ostream>

enum class ProcessState
{
    READY = 0,
    RUNNING = 1,
    WAITING_IO = 2,
    FINISHED = 3
};

class PCB
{
    friend std::ostream& operator<<(std::ostream& out, const PCB& p);

   public:
    PCB(int pid,
        int prio,
        int burst,
        bool io_bound,
        int io_interval,
        int aging_threshold,
        int time_quantum);

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
    int getTotalCpuUsedTime() const;
    int getFirstResponseTime() const;
    bool isIOBound() const;
    bool isFirstResponse() const;

    ProcessState getState() const;
    std::string getStringState() const;

    // Setters
    void setState(ProcessState state);
    void setPriority(int prio);
    void recordFirstResponse(int time);
    void setIOTime(int io_remainingtime);
    void setCompletionTime(int time);
    void incrementTotalIO(int time);
    void resetIO();
    void startIO();

    // Boolean helpers for state
    bool isReady() const;
    bool isRunning() const;
    bool isWaitingIO() const;
    bool isFinished() const;

   private:
    int pid;                  // process id
    int prio;                 // current priority
    int old_prio;             // original prio
    int burst_time;           // burst time
    int remaining_time;       // remaining time after burst
    int waiting_time;         // time a process has waited to be executed (for aging)
    bool io_bound;            // true if process is I/O bound
    int io_interval;          // how often in ms it performs I/O
    int io_remaining;         // how long left of io operation
    int cpu_used_on_process;  // CPU time used total between i/o interrupt
    int total_cpu_used;       // total cpu time used
    int completion_time;      // set this, when process completes. Updates in scheduler
    int total_io_time;        // total time spent in i/o wait. Updates in IOManager
    int first_response_time;  // note the time of the first response for process.
    bool first_response;      // set this, for first execute cycle on the process
    ProcessState PS;          // Process State

    // scheduler config
    int pcb_aging_t;
    int pcb_time_q;
};