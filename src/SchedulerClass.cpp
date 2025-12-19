#include "SchedulerClass.h"
#include "LogsJson.h"
#include "PCB.h"
#include <climits>
#include <memory>
#include <sstream>

Scheduler::Scheduler(std::string logs_name) : logs_name(logs_name) {
  wait_time.resize(N, 0);
  time_pid = 0;
  total_wait_time = 0;
  createJSON(logs_name);
}

void Scheduler::findWaitTime() {
  wait_time[0] = 0; // set to 0, because first element has no waittime
}

void Scheduler::findTotalTime() {}

void Scheduler::findAvgTime() {
  std::unique_lock<std::mutex> lk(schedule_lock);
  findWaitTime();
  findTotalTime();
}

void Scheduler::priorityScheduling() {
  std::unique_lock<std::mutex> lk(schedule_lock);
  std::sort(process_pool.begin(), process_pool.end(),
            [](const std::unique_ptr<PCB> &a, const std::unique_ptr<PCB> &b) {
              return a->getPriority() < b->getPriority();
            });

  debug(EXEC, "Process order");
  for (const auto &e : process_pool) {
    debug(EXEC, "PID: " + std::to_string(e->getPid()) +
                    ", burst time: " + std::to_string(e->getBurstTime()) +
                    ", priority: " + std::to_string(e->getPriority()));
  }
}

/*
bool Scheduler::ageProcess(std::unique_ptr<PCB> &proc, int time_left) {

  proc.waiting_time += time_left;
  debug(AGING, "[AGING] " + std::to_string(proc.pid) +
                   " aged, waiting time: " + std::to_string(proc.waiting_time) +
                   " remaining time: " + std::to_string(proc.remainingtime));

  // Aging threshold proportional with time quantum. (40 sec)
  if (proc.waiting_time >= 5 * TIME_QUANTUM && proc.prio > 1) {
    proc.old_prio = proc.prio;
    proc.prio--;
    proc.waiting_time = 0;
    debug(AGING, [&]() {
      std::ostringstream oss;
      oss << "[UPGRADE] - process with pid: " << proc.pid
          << " with old prio: " << proc.old_prio
          << " upgraded to prio: " << proc.prio;
      return oss.str();
    });
    return true;
  }
  return false;
}


bool Scheduler::executeProcess(Process &p, int &currentTime) {

  int executeTime = 0;

  if (p.io_bound) {
    executeTime =
        std::min({TIME_QUANTUM, p.remainingtime, (p.io_interval - p.cpu_used)});
  } else {
    executeTime = std::min(TIME_QUANTUM, p.remainingtime);
  }

  if (executeTime <= 0) {
    debug(WARNING,
          "[WARNING] no time to execute for PID: " + std::to_string(p.pid));
    return false;
  }

  currentTime += executeTime;
  p.remainingtime -= executeTime;
  p.cpu_used += executeTime;

  debug(EXEC, [&]() {
    std::ostringstream oss;
    oss << "[EXEC] PID " << p.pid << " ran for " << executeTime
        << " -> remaining: " << p.remainingtime
        << " currentTime: " << currentTime;
    return oss.str();
  });

  if (p.remainingtime <= 0) {
    debug(EXEC, [&]() {
      std::ostringstream oss;
      oss << "[FINISHING] PID: " << p.pid << " with prio: " << p.prio
          << " spent " << currentTime << " time executing";
      return oss.str();
    });

    p.remainingtime = 0;
    return false;
  }

  else if (p.io_bound && p.cpu_used >= p.io_interval) {
    debug(IO, "PID : " + std::to_string(p.pid));
    return false;
  }

  return true; // requeue process (not finished)
}

void Scheduler::updateIO(Process &p) {
  if (p.cpu_used >= p.io_interval && p.io_bound && p.remainingtime > 0) {
    // compute the index of process p within the processes vector
    size_t idx = &p - &processes[0];
    if (readyQueue[p.prio].contains(idx)) {
      if (!readyQueue[p.prio].remove(idx)) {
        debug(WARNING, "[WARNING] PID : " + std::to_string(p.pid) +
                           " not found in ready queue for removal");
      }
    }

    p.waiting_io = true;
    p.io_remaining = p.io_interval;
    p.cpu_used = 0;
    io_waitQueue.push_back(idx);
    debug(IO, "[IO PUSH] PID : " + std::to_string(p.pid) +
                  " entering IO wait for " + std::to_string(p.io_remaining));
  }
}

void Scheduler::processIO(int &time_diff, int &currentTime) {
  // Phase 1 -> Find finished i/O processes

  time_diff = std::max(0, time_diff); // secure time_diff is positive.

  std::vector<size_t> temp_io;
  for (auto idx : io_waitQueue) {
    auto &proc = processes[idx];
    if (proc.io_remaining > 0) {
      proc.io_remaining = std::max(0, proc.io_remaining - time_diff);
    }

    if (proc.io_remaining <= 0) {
      temp_io.push_back(idx);
    }
  }

  handleIOqueue(temp_io, currentTime);
}

void Scheduler::handleIOqueue(std::vector<size_t> temp_io, int &currentTime) {
  // Phase 2 -> Reinsert, delete and cleanup.
  if (io_waitQueue.empty()) {
    return;
  }

  for (auto idx : temp_io) {
    auto &done_proc = processes[idx];
    done_proc.waiting_io = false;
    done_proc.cpu_used = 0;
    done_proc.io_remaining = 0;
    done_proc.waiting_time = 0;

    if (done_proc.remainingtime > 0) {
      debug(IO, "[IO DONE] PID " + std::to_string(done_proc.pid) +
                    " resuming from IO at time " + std::to_string(currentTime));
      readyQueue[done_proc.prio].push(idx);
    }
    // erase-remove idiom
    io_waitQueue.erase(
        std::remove(io_waitQueue.begin(), io_waitQueue.end(), idx),
        io_waitQueue.end());
  }
}
*/

void Scheduler::updateQueuesAfterAging(std::unique_ptr<PCB> &p,
                                       int &time_slice) {
  //***** Update aging *****//
  // phase 1 - find aged processes, and store them in vector
  std::vector<size_t> aged_processes;
  for (size_t idx = 0; idx < process_pool.size(); idx++) {
    if (process_pool[idx]->getRemainingTime() > 0 &&
        process_pool[idx]->getPid() != p->getPid() &&
        !process_pool[idx]->isWaitingIO() &&
        readyQueue[process_pool[idx]->getPriority()].contains(idx)) {
      if (process_pool[idx]->ageProcess(time_slice)) {
        aged_processes.push_back(idx);
      }
    }
  }
  // phase 2 - remove and push the aged processes, to appropriate queues.
  for (auto id : aged_processes) {
    readyQueue[process_pool[id]->getOldPriority()].remove(id);
    readyQueue[process_pool[id]->getPriority()].push(id);
  }
}

void Scheduler::logEvent(std::unique_ptr<PCB> &p) {
  enum class Event { RUNNING, IO_WAIT, FINISHED };

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
                  {"CPU used", p->getCpuUsed()}};

  eventLog.push_back(j_array);
}

void Scheduler::flushLogs() {
  for (auto &e : eventLog) {
    appendToJSON(logs_name, e);
  }
}

bool Scheduler::cleanUpQueues(int &currentTime, int &lastTime) {
  bool hasRemainingWork = false;
  for (const auto &proc : process_pool) {
    if (proc->getRemainingTime() > 0) {
      hasRemainingWork = true;
      break;
    }
  }

  // If this condition is met, all processes done.
  if (!hasRemainingWork && IO_Processes->isEmpty()) {
    lastTime = currentTime;
    return false;
  }

  // Check if Ready Queue has processes to process.
  bool anyQueueHasWork = false;
  for (int el = 1; el <= MAX_PRIORITY; ++el) {
    if (!readyQueue[el].empty()) {
      anyQueueHasWork = true;
      break;
    }
  }

  // If all processes are in IO wait, advance time
  if (!anyQueueHasWork && !IO_Processes->isEmpty()) {
    // Find minimum IO remaining time
    int minTime = IO_Processes->getMinRemainingIOTime();

    debug(QUEUE, "All processes in IO wait, advancing time by " +
                     std::to_string(minTime) + "ms");
    currentTime += minTime;

    // Process IO completions
    std::vector<std::unique_ptr<PCB>> temp_ready; // hold finished I/O process

    for (auto &pcb_ptr : IO_Processes->getQueue()) {
      pcb_ptr->setIOTime(pcb_ptr->getIORemainingTime() - minTime);
      if (pcb_ptr->getIORemainingTime() <= 0) {
        temp_ready.push_back(std::move(pcb_ptr));
      }
    }

    std::vector<size_t> temp_io;
    for (auto idx : io_waitQueue) {
      auto &proc = processes[idx];

      proc.io_remaining -= minTime;
      if (proc.io_remaining <= 0) {
        temp_io.push_back(idx);
      }
    }

    // Reinsert completed IO processes
    for (auto idx : temp_io) {
      auto &done_proc = processes[idx];

      done_proc.waiting_io = false;
      done_proc.cpu_used = 0;
      done_proc.io_remaining = 0;
      done_proc.waiting_time = 0;

      if (done_proc.remainingtime > 0) {
        debug(QUEUE, "[IO DONE] PID " + std::to_string(done_proc.pid) +
                         " resuming from IO at time " +
                         std::to_string(currentTime));
        readyQueue[done_proc.prio].push(idx);
      }

      io_waitQueue.erase(
          std::remove(io_waitQueue.begin(), io_waitQueue.end(), idx),
          io_waitQueue.end());
    }

    lastTime = currentTime;

    for (size_t i = 0; i < processes.size(); ++i) {
      const auto &p = processes[i];
      if (p.remainingtime > 0) {
        debug(WARNING, [&]() {
          std::ostringstream oss;
          oss << "[STUCK PROC] PID: " << p.pid
              << " remaining: " << p.remainingtime
              << " waiting_io: " << p.waiting_io << " prio: " << p.prio
              << " contained in readyQueue? " << std::boolalpha
              << readyQueue[p.prio].contains(i);
          return oss.str();
        });
      }
    }
    debug(WARNING,
          "[WARNING] Stalled: no ready or IO processes but work remains!");
    return false; // prevents infinite loop
  }
  return true;
}

void Scheduler::roundRobin() {
  std::unique_lock<std::mutex> lk(schedule_lock);

  for (size_t p = 0; p < processes.size(); p++) {
    if (processes[p].prio < 0 || processes[p].prio > MAX_PRIORITY) // use clamp!
    {
      processes[p].prio = std::clamp(processes[p].prio, 1, MAX_PRIORITY);
      debug(WARNING, "Clamping prio - otherwise out of index");
    }
    readyQueue[processes[p].prio].push(p);
  }

  int currentTime = 0; // track current time
  int lastTime = 0;

  while (true) {
    if (!cleanUpQueues(currentTime, lastTime)) {
      break;
    }

    // Execute process
    for (auto el = 1; el <= MAX_PRIORITY; ++el) {
      if (!readyQueue[el].empty()) {

        debug(QUEUE, [&]() {
          std::ostringstream oss;
          oss << "=== Queue status at time " << currentTime << " ===\n";
          for (int prio = 1; prio <= MAX_PRIORITY; ++prio) {
            if (!readyQueue[prio].empty()) {
              oss << "Priority " << prio << ": " << readyQueue[prio] << "\n";
            }
            oss << "IO wait queue size: " << io_waitQueue.size()
                << "\n========================\n";
          }
          return oss.str();
        });

        if (readyQueue.empty())
          continue;
        size_t i = readyQueue[el].pop();
        auto &p = processes[i];

        //***** Execute process *****//

        bool shouldRequeue = executeProcess(p, currentTime);
        if (shouldRequeue && p.remainingtime > 0 && !p.waiting_io)
          readyQueue[el].push(i);

        //***** Update aging *****//
        int delta = std::min(currentTime - lastTime, TIME_QUANTUM);
        updateQueuesAfterAging(p, delta);

        //***** Update IO Wait Queue here! *****//
        updateIO(p);

        //***** IO Wait Queue management! *****//
        processIO(delta, currentTime);

        lastTime = currentTime;
        //***** Update logs *****//
        logEvent(p);
        break;
      }
    }
  }

  // when finished write all to logs.
  debug(EXEC, "Flushing logs");
  flushLogs();
}

void Scheduler::run() {
  // std::chrono::steady_clock::time_point begin =
  // std::chrono::steady_clock::now();

  enum Process_STATE {
    READY,
    RUNNING,
    FINISHED,
  };
  Process_STATE curr_state = Process_STATE::READY;

  switch (curr_state) {
  case Process_STATE::READY:
    // std::cout << "IN ready state" << std::endl;
    curr_state = Process_STATE::RUNNING;

  case Process_STATE::RUNNING:
    // std::cout << "IN running state" << std::endl;
    priorityScheduling();
    roundRobin();
    curr_state = Process_STATE::FINISHED;

  case Process_STATE::FINISHED:
    // std::cout << "IN finished state" << std::endl;
    // findAvgTime();
    // std::chrono::steady_clock::time_point end =
    // std::chrono::steady_clock::now(); debug(EXEC,
    // std::chrono::duration_cast<std::chrono::milliseconds>(end -
    // begin).count());
    debug(EXEC, "Finished");
  }
}

void Scheduler::simulateTime(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Scheduler::setDebugFlags(int flags) { this->debug_level = flags; }

void Scheduler::enableDebug(DebugLevel category) {
  debug_level |= category; // Logical OR
}

void Scheduler::disableDebug(DebugLevel category) {
  debug_level &= ~category; // Logical AND with one's complement (bitwise NOT)
}

bool Scheduler::isDebugEnabled(DebugLevel category) const {
  return ((debug_level & category) != 0);
}

int Scheduler::getDebugLevel() const { return debug_level; }
