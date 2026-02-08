#pragma once
#include "IOManager.h"
#include "LogsJson.h"
#include "PCB.h"
#include "ReadyQueue.h"
#include <array>
#include <iostream>
#include <string>
#include <vector>

constexpr int MAX_PRIORITY = 3;

class Scheduler {
public:
  Scheduler(std::string logs_name);

  // Debug logger
  enum DebugLevel {
    NONE = 0,         // 0
    EXEC = 1 << 0,    // 1
    AGING = 1 << 1,   // 2
    IO = 1 << 2,      // 4
    QUEUE = 1 << 3,   // 8
    WARNING = 1 << 4, // 16
    ALL = 0xFFFF
  };

  // Debug
  void setDebugFlags(int flags);
  void enableDebug(DebugLevel level);
  void disableDebug(DebugLevel level);
  bool isDebugEnabled(DebugLevel level) const;
  int getDebugLevel() const;

  // Time tracking -> Not implemented properly.
  void findWaitTime();
  void findTurnaroundTime();
  void findResponseTime();

  // Queue handler
  void updateQueuesAfterAging(PCB *p, int &time_slice);

  // Logging
  void logEvent(PCB *p);
  void flushLogs();

  // Scheduling + Queues setup.
  void priorityScheduling();
  bool cleanUpQueues(int &currentTime, int &lastTime);
  void roundRobin();

  // Statemachine
  void run();

  void simulateTime(int ms);
  ~Scheduler() = default;

private:
  // Helper functions to debug

  // Debug helper for lambdas
  template <typename Func>
  auto debug(DebugLevel category, Func &&func) const
      -> decltype(func(), void()) {
    if (debug_level & category) {
      std::cout << func() << std::endl;
    }
  }

  // Debug helper for direct values
  template <typename T>
  auto debug(DebugLevel category, T &&msg) const
      -> decltype(std::cout << msg, void()) {
    if (debug_level & category) {
      std::cout << std::forward<T>(msg) << std::endl;
    }
  }

  // Default debug-level set to NONE
  int debug_level = NONE;

  // Helper methods
  PCB &getProcessByPID(int pid);
  size_t pidToIndex(int pid) const;
  int indexToPid(size_t idx) const;

  // clang-format off
  std::array<PCB, N> process_pool = {
      PCB(1, 2, 10, 1, 5),
      PCB(2, 3, 8, 0, 0),
      PCB(3, 1, 22, 1, 9),
      PCB(4, 2, 12, 0, 0),
      PCB(5, 3, 19, 0, 0),
      PCB(6, 1, 5, 0, 0),
  };
  // clang-format on

  std::vector<int> wait_time;
  std::array<ReadyQueue<size_t, N * 2>, MAX_PRIORITY + 1> readyQueue;
  IOManager IO_Processes;

  std::string logs_name;
  std::vector<json> eventLog; // store json objects for logging
};
