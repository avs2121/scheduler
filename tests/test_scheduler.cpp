#include <LogsJson.h>

#include "SchedulerClass.h"
#include "TestFixture.h"

const auto config_path = std::filesystem::current_path() / "config" / "process_config.json";

class SchedulerConstructionTest : public TestFixture
{
   public:
    SchedulerConstructionTest() : TestFixture("Scheduler Construction")
    {
    }

    void test()
    {
        std::string logfile = "test_construction";
        trackFile(logfile);

        assert_true(std::filesystem::exists(config_path), "Config path must exist");

        Scheduler schedule(logfile, config_path.string());
        assert_true(fileExists(logfile), "Logfile doesnt exist");
    }
};

class SchedulerDebugTest : public TestFixture
{
   public:
    SchedulerDebugTest() : TestFixture("Debug Test")
    {
    }

    void test()
    {
        std::string logfile = "debug_test";
        trackFile(logfile);

        assert_true(std::filesystem::exists(config_path), "Config path must exist");

        Scheduler scheduler(logfile, config_path.string());

        scheduler.enableDebug(Scheduler::EXEC);
        assert_true(scheduler.isDebugEnabled(Scheduler::EXEC), "Debug should be enabled");

        scheduler.disableDebug(Scheduler::EXEC);
        assert_true(!scheduler.isDebugEnabled(Scheduler::EXEC), "Debug should be disabled");

        scheduler.setDebugFlags(Scheduler::EXEC);
        assert_true(scheduler.isDebugEnabled(Scheduler::EXEC), "Debug should be enabled again");
    }
};

class SchedulerFullRunTest : public TestFixture
{
   public:
    SchedulerFullRunTest() : TestFixture("Full Scheduler Run Test")
    {
    }

    void test()
    {
        std::string logfile = "full_test";
        trackFile(logfile);

        assert_true(std::filesystem::exists(config_path), "Config path must exist");

        Scheduler scheduler(logfile, config_path.string());

        scheduler.run();
        assert_true(fileExists(logfile), "Log file doesnt exists");
        assert_true(countLines(logfile) > 0, "Log file contains lines");
    }
};

void run_scheduler_tests()
{
    std::cout << "\n==== Scheduler Test ====\n";
    int passed = 0, failed = 0;

    try
    {
        SchedulerConstructionTest test1;
        test1.run([&]() { test1.test(); });
        passed++;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception raised in [CONSTRUCTION TEST] with: " << e.what() << std::endl;
        failed++;
    }

    try
    {
        SchedulerDebugTest test2;
        test2.run([&]() { test2.test(); });
        passed++;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception raised in [DEBUG TEST] with: " << e.what() << std::endl;
        failed++;
    }

    try
    {
        SchedulerFullRunTest test3;
        test3.run([&]() { test3.test(); });
        passed++;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception raised in [FULL RUN TEST] with: " << e.what() << std::endl;
        failed++;
    }
    std::cout << "Scheduler test passed with: " << passed << " passed , " << failed << " failed"
              << std::endl;
}