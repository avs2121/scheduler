#include "SchedulerClass.h"
#include <thread>

int main() {
  Scheduler scheduler("proces_logs");
  scheduler.setDebugFlags(Scheduler::EXEC | Scheduler::AGING |
                          Scheduler::WARNING);

  std::thread t1([&scheduler]() { scheduler.run(); });

  t1.join();

  return 0;
}