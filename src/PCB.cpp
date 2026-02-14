#include "PCB.h"

#include <algorithm>

PCB::PCB(int pid, int prio, int burst, bool io_bound, int io_interval, int aging_threshold, int time_quantum) : pid(pid), prio(prio), burst_time(burst), io_bound(io_bound), io_interval(io_interval)
{
    this->old_prio = prio;
    this->remaining_time = burst;
    this->waiting_time = 0;
    this->io_remaining = 0;
    this->cpu_used_on_process = 0;
    this->total_cpu_used = 0;
    this->completion_time = -1;
    this->total_io_time = 0;
    this->first_response = false;
    this->first_response_time = -1;
    this->PS = ProcessState::READY;

    this->pcb_aging_t = aging_threshold;
    this->pcb_time_q = time_quantum;
}

int PCB::execute(int timeslice)
{
    int executeTime = computeExecuteTime(timeslice);

    if (executeTime <= 0)
    {
        return 0;
    }

    PS = ProcessState::RUNNING;
    remaining_time -= executeTime;
    cpu_used_on_process += executeTime;

    updateStatesAfterExecution();

    return executeTime;
}

int PCB::computeExecuteTime(int timeslice)
{
    if (io_bound)
    {
        return std::min({timeslice, remaining_time, (io_interval - cpu_used_on_process)});
    }
    else
    {
        return std::min(timeslice, remaining_time);
    }
}

bool PCB::ageProcess(int timediff)
{
    waiting_time += timediff;

    // Aging threshold proportional with time quantum. (5 * 4)
    if (waiting_time >= pcb_aging_t * pcb_time_q && prio > 1)
    {
        old_prio = prio;
        prio--;
        waiting_time = 0;
        return true;
    }
    return false;
}

void PCB::updateStatesAfterExecution()
{
    if (remaining_time <= 0)
    {
        PS = ProcessState::FINISHED;  // process is finished executing.
        total_cpu_used += cpu_used_on_process;
        remaining_time = 0;
    }

    else if (io_bound && cpu_used_on_process >= io_interval)
    {
        PS = ProcessState::WAITING_IO;  // process IO-interval reached go into wait.
    }

    else
    {
        PS = ProcessState::READY;  // requeue process (not finished).
    }
}

int PCB::getPid() const
{
    return pid;
}

int PCB::getPriority() const
{
    return prio;
}

int PCB::getOldPriority() const
{
    return old_prio;
}

int PCB::getBurstTime() const
{
    return burst_time;
}

int PCB::getCpuUsed() const
{
    return cpu_used_on_process;
}

int PCB::getIOInterval() const
{
    return io_interval;
}

int PCB::getRemainingTime() const
{
    return remaining_time;
}

int PCB::getIORemainingTime() const
{
    return io_remaining;
}

int PCB::getWaitingTime() const
{
    return waiting_time;
}

int PCB::getCompletionTime() const
{
    return completion_time;
}

int PCB::getTotalIOTime() const
{
    return total_io_time;
}

int PCB::getTotalCpuUsedTime() const
{
    return total_cpu_used;
}

int PCB::getFirstResponseTime() const
{
    return first_response_time;
}

ProcessState PCB::getState() const
{
    return PS;
}

std::string PCB::getStringState() const
{
    switch (PS)
    {
        case ProcessState::READY:
            return "READY";
            break;
        case ProcessState::RUNNING:
            return "RUNNING";
            break;
        case ProcessState::WAITING_IO:
            return "WAITING IO";
            break;
        case ProcessState::FINISHED:
            return "FINISHED";
            break;
        default:
            return "UNKNOWN";
            break;
    }
}

bool PCB::isIOBound() const
{
    return io_bound;
}

bool PCB::isFirstResponse() const
{
    return first_response;
}

void PCB::setState(ProcessState state)
{
    this->PS = state;
}

void PCB::setPriority(int prio_)
{
    this->prio = prio_;
}

void PCB::recordFirstResponse(int time)
{
    this->first_response = true;
    this->first_response_time = time;
}

void PCB::setIOTime(int io_remainingtime_)
{
    this->io_remaining = io_remainingtime_;
}

void PCB::setCompletionTime(int time)
{
    this->completion_time = time;
}

void PCB::incrementTotalIO(int time)
{
    this->total_io_time += time;
}

void PCB::resetIO()
{
    this->io_remaining = 0;
    this->waiting_time = 0;
    this->PS = ProcessState::READY;
}

void PCB::startIO()
{
    this->total_cpu_used += cpu_used_on_process;
    this->io_remaining = io_interval;
    this->cpu_used_on_process = 0;
    this->PS = ProcessState::WAITING_IO;
}

bool PCB::isReady() const
{
    return PS == ProcessState::READY;
}

bool PCB::isRunning() const
{
    return PS == ProcessState::RUNNING;
}

bool PCB::isWaitingIO() const
{
    return PS == ProcessState::WAITING_IO;
}

bool PCB::isFinished() const
{
    return PS == ProcessState::FINISHED;
}

std::ostream& operator<<(std::ostream& out, const PCB& p)
{
    out << "{PID:" << p.pid << ", Prio:" << p.prio << ", Burst:" << p.burst_time << ", Remain:" << p.remaining_time << ", Waited: " << p.waiting_time << ", io bound: " << std::boolalpha << p.io_bound
        << ", io interval: " << p.io_interval << ", cpu used: " << p.cpu_used_on_process << ", state: " << p.getStringState() << "}";
    return out;
}
