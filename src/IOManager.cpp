#include "IOManager.h"
#include <algorithm>
#include <array>
#include <climits>
#include <iostream>

IOManager::IOManager(std::array<PCB, N> &process_pool)
    : process_pool(process_pool) {}

void IOManager::enqueue(size_t idx) { IO_queue.push_back(idx); }

void IOManager::updateIO() {

  if (IO_queue.empty())
    return;

  for (size_t idx : IO_queue) {
    PCB &proc = process_pool[idx];

    if (proc.isWaitingIO() && proc.isIOBound() && proc.getRemainingTime() > 0) {
      proc.startIO();
    }
  }
}

void IOManager::processIO(int timeslice) {
  // Phase 1 -> Find finished i/O processes
  if (IO_queue.empty())
    return;

  int time_diff = std::max(0, timeslice); // secure time_diff is positive.

  for (size_t idx : IO_queue) {
    PCB &proc = process_pool[idx];
    if (proc.getIORemainingTime() > 0) {
      proc.setIOTime(std::max(0, proc.getIORemainingTime() - time_diff));
    }

    if (proc.getIORemainingTime() <= 0) {
      finished_IO.push_back(idx);
    }
  }

  handleIOqueue();
}

void IOManager::handleIOqueue() {
  // Phase 2 -> Reinsert, delete and cleanup.
  if (IO_queue.empty())
    return;

  for (size_t idx : finished_IO) {
    PCB &proc = process_pool[idx];
    proc.resetIO();
  }

  // remove all finished process, by detecting waiting_IO flag.
  IO_queue.erase(std::remove_if(IO_queue.begin(), IO_queue.end(),
                                [&](size_t idx) {
                                  return !(process_pool[idx].isWaitingIO());
                                }),
                 IO_queue.end());
}

const std::vector<size_t> &IOManager::getFinishedProcesses() const {
  return finished_IO;
}

bool IOManager::isEmpty() const { return IO_queue.empty(); }

size_t IOManager::size() const { return IO_queue.size(); }

bool IOManager::containsPID(int pid) const {
  return (
      std::any_of(IO_queue.begin(), IO_queue.end(), [pid, this](size_t idx) {
        return pid == process_pool[idx].getPid();
      }));
}

void IOManager::clear() {
  IO_queue.clear();
  finished_IO.clear();
}

int IOManager::getMinRemainingIOTime() const {
  int min = INT_MAX;
  for (size_t idx : IO_queue) {
    PCB &proc = process_pool[idx];
    if (proc.getIORemainingTime() < min) {
      min = proc.getIORemainingTime();
    }
  }

  return min;
};

const std::deque<size_t> &IOManager::getQueue() const { return IO_queue; }

void IOManager::printQueue() const {
  for (const auto &p : IO_queue) {
    std::cout << p << std::endl;
  }
}
