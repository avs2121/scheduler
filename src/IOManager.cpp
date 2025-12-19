#include "IOManager.h"
#include <algorithm>
#include <climits>
#include <iostream>

void IOManager::enqueue(std::unique_ptr<PCB> p) {
  if (p)
    IO_queue.push_back(std::move(p));
}

void IOManager::updateIO() {

  if (IO_queue.empty())
    return;

  for (auto &pcb_ptr : IO_queue) {
    if (pcb_ptr->isWaitingIO() && pcb_ptr->isIOBound() &&
        pcb_ptr->getRemainingTime() > 0) {
      pcb_ptr->startIO();
    }
  }
}

void IOManager::processIO(int timeslice) {
  // Phase 1 -> Find finished i/O processes
  if (IO_queue.empty())
    return;

  int time_diff = std::max(0, timeslice); // secure time_diff is positive.

  for (auto &pcb_ptr : IO_queue) {
    if (pcb_ptr->getIORemainingTime() > 0) {
      pcb_ptr->setIOTime(
          std::max(0, pcb_ptr->getIORemainingTime() - time_diff));
    }

    if (pcb_ptr->getIORemainingTime() <= 0) {
      finished_IO.push_back(std::move(pcb_ptr));
    }
  }

  handleIOqueue();
}

void IOManager::handleIOqueue() {
  // Phase 2 -> Reinsert, delete and cleanup.
  if (IO_queue.empty())
    return;

  for (auto &pcb_ptr : finished_IO) {
    pcb_ptr->resetIO();
  }

  // remove all finished process, by detecting waiting_IO flag.
  IO_queue.erase(std::remove_if(IO_queue.begin(), IO_queue.end(),
                                [](std::unique_ptr<PCB> &pcb_ptr) {
                                  return !(pcb_ptr->isWaitingIO());
                                }),
                 IO_queue.end());
}

const std::vector<std::unique_ptr<PCB>> &
IOManager::getFinishedProcesses() const {
  return finished_IO;
}

bool IOManager::isEmpty() const { return IO_queue.empty(); }

size_t IOManager::size() const { return IO_queue.size(); }

bool IOManager::contains(const PCB *p) const {
  if (!p) {
    return false;
  }
  return (std::any_of(
      IO_queue.begin(), IO_queue.end(),
      [p](const std::unique_ptr<PCB> &pcb_ptr) { return p == pcb_ptr.get(); }));
}

bool IOManager::containsPID(int pid) const {
  for (const auto &process : IO_queue) {
    if (process->getPid() == pid) {
      return true;
    }
  }
  return false;
}

void IOManager::clear() {
  IO_queue.clear();
  finished_IO.clear();
}

int IOManager::getMinRemainingIOTime() const {
  int min = INT_MAX;
  for (const auto &p : IO_queue) {
    if (p->getIORemainingTime() < min) {
      min = p->getIORemainingTime();
    }
  }

  return min;
};

const std::deque<std::unique_ptr<PCB>> &IOManager::getQueue() const {
  return IO_queue;
}

void IOManager::printQueue() const {
  for (const auto &p : IO_queue) {
    std::cout << p << std::endl;
  }
}
