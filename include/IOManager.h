#pragma once
#include "PCB.h"
#include <deque>
#include <memory>
#include <vector>


class IOManager {
public:
  IOManager() = default;

  void enqueue(std::unique_ptr<PCB> p);
  void updateIO();
  void processIO(int timeslice);
  void handleIOqueue();
  const std::vector<std::unique_ptr<PCB>> &getFinishedProcesses() const;

  // Query methods
  bool isEmpty() const;
  size_t size() const;
  bool contains(const PCB *p) const;
  bool containsPID(int pid) const;

  // Utility methods
  void clear();
  int getMinRemainingIOTime() const;

  // Debug funcs
  const std::deque<std::unique_ptr<PCB>> &getQueue() const;
  void printQueue() const;

private:
  std::deque<std::unique_ptr<PCB>> IO_queue;
  std::vector<std::unique_ptr<PCB>> finished_IO;
};
