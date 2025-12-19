#include "PCB.h"

PCB::PCB(int pid, int prio, int burst, bool io_bound, int io_interval)
    : pid(pid), prio(prio), bursttime(burst), io_bound(io_bound),
      io_interval(io_interval) {
  this->old_prio = prio;
  this->remainingtime = burst;
  this->waiting_time = 0;
  this->io_remaining = 0;
  this->cpu_used = 0;
  this->PS = ProcessState::READY;
}

bool PCB::execute(int timeslice) {
  int executeTime = 0;

  if (io_bound) {
    executeTime =
        std::min({timeslice, remainingtime, (io_interval - cpu_used)});
  } else {
    executeTime = std::min(timeslice, remainingtime);
  }

  if (executeTime <= 0) {
    return false;
  }

  PS = ProcessState::RUNNING;
  remainingtime -= executeTime;
  cpu_used += executeTime;

  if (remainingtime <= 0) {

    PS = ProcessState::FINISHED;
    remainingtime = 0;
    return false;
  }

  else if (io_bound && cpu_used >= io_interval) {
    PS = ProcessState::WAITING_IO;
    return false;
  }

  PS = ProcessState::READY;
  return true; // requeue process (not finished)
}

bool PCB::ageProcess(int timediff) {
  waiting_time += timediff;

  // Aging threshold proportional with time quantum. (40 sec)
  if (waiting_time >= 5 * TIME_QUANTUM && prio > 1) {
    old_prio = prio;
    prio--;
    waiting_time = 0;
    return true;
  }
  return false;
}

int PCB::getPid() const { return pid; }

int PCB::getPriority() const { return prio; }

int PCB::getOldPriority() const { return old_prio; }

int PCB::getBurstTime() const { return bursttime; }

int PCB::getCpuUsed() const { return cpu_used; }

int PCB::getIOInterval() const { return io_interval; }

int PCB::getRemainingTime() const { return remainingtime; }

int PCB::getIORemainingTime() const { return io_remaining; }

int PCB::getWaitingTime() const { return waiting_time; }

ProcessState PCB::getState() const { return PS; }

std::string PCB::getStringState() const {
  switch (PS) {
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

bool PCB::isIOBound() const { return io_bound; }

void PCB::setState(ProcessState state) { this->PS = state; }

void PCB::setPriority(int prio_) { this->prio = prio_; }

void PCB::setIOTime(int io_remainingtime_) {
  this->io_remaining = io_remainingtime_;
}

void PCB::resetIO() {
  this->cpu_used = 0;
  this->io_remaining = 0;
  this->waiting_time = 0;
  this->PS = ProcessState::READY;
}

void PCB::startIO() {
  this->io_remaining = io_interval;
  this->cpu_used = 0;
  this->PS = ProcessState::WAITING_IO;
}

bool PCB::isReady() const { return PS == ProcessState::READY; }

bool PCB::isRunning() const { return PS == ProcessState::RUNNING; }

bool PCB::isWaitingIO() const { return PS == ProcessState::WAITING_IO; }

bool PCB::isFinished() const { return PS == ProcessState::FINISHED; }

std::ostream &operator<<(std::ostream &out, const PCB &p) {
  out << "{PID:" << p.pid << ", Prio:" << p.prio << ", Burst:" << p.bursttime
      << ", Remain:" << p.remainingtime << ", Waited: " << p.waiting_time
      << ", io bound: " << std::boolalpha << p.io_bound
      << ", io interval: " << p.io_interval << ", cpu used: " << p.cpu_used
      << ", state: " << p.getStringState() << "}";
  return out;
}
