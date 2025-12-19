#include "SchedulerClass.h"
#include "TestFixture.h"

class SchedulerConstructionTest : public TestFixture {
public:
  SchedulerConstructionTest() : TestFixture("Scheduler Construction") {}

  void test() {
    std::string logfile = "test_construction.json";
    trackFile(logfile);

    Scheduler schedule(logfile);
    assert_true(fileExists(logfile), "Logfile should exist");
  }
};

class SchedulerDebugTest : public TestFixture {
public:
  SchedulerDebugTest() : TestFixture("Debug Test") {}

  void test() {
    std::string logfile = "debug_test.json";
    trackFile(logfile);
    Scheduler scheduler(logfile);

    scheduler.enableDebug(Scheduler::EXEC);
    assert_true(scheduler.isDebugEnabled(Scheduler::EXEC),
                "Debug should be enabled");

    scheduler.disableDebug(Scheduler::EXEC);
    assert_true(!scheduler.isDebugEnabled(Scheduler::EXEC),
                "Debug should be disabled");

    scheduler.setDebugFlags(Scheduler::EXEC);
    assert_true(scheduler.isDebugEnabled(Scheduler::EXEC),
                "Debug should be enabled again");
  }
};

class SchedulerFullRunTest : public TestFixture {
public:
  SchedulerFullRunTest() : TestFixture("Full Scheduler Run Test") {}

  void test() {
    std::string logfile = "full_test.json";
    trackFile(logfile);
    Scheduler scheduler(logfile);

    scheduler.run();
    assert_true(fileExists(logfile), "Log file exists");
    assert_true(countLines(logfile) > 0, "Log file contains lines");
  }
};

void run_scheduler_tests() {
  std::cout << "\n==== Scheduler Test ====\n";
  int passed = 0, failed = 0;

  try {
    SchedulerConstructionTest test1;
    test1.run([&]() { test1.test(); });
    passed++;
  } catch (std::exception &e) {
    std::cout << "Exception raised in [CONSTRUCTION TEST] with: " << e.what()
              << std::endl;
    failed++;
  }

  try {
    SchedulerDebugTest test2;
    test2.run([&]() { test2.test(); });
    passed++;
  } catch (std::exception &e) {
    std::cout << "Exception raised in [DEBUG TEST] with: " << e.what()
              << std::endl;
    failed++;
  }

  try {
    SchedulerFullRunTest test3;
    test3.run([&]() { test3.test(); });
    passed++;
  } catch (std::exception &e) {
    std::cout << "Exception raised in [FULL RUN TEST] with: " << e.what()
              << std::endl;
    failed++;
  }
  std::cout << "Scheduler test passed with: " << passed << " passed , "
            << failed << " failed" << std::endl;
}