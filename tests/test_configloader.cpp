#include <LogsJson.h>

#include "ConfigLoader.h"
#include "TestFixture.h"

struct testSchedulerConfig
{
    int aging_threshold = 5;
    int max_priority = 3;
    int time_quantum = 4;
};

struct testProcessConfig
{
    int pid = 1;
    int priority = 2;
    int burst = 5;
    bool io_bound = true;
    int io_interval = 4;
};

class ConfigLoaderConstructionTest : public TestFixture
{
   public:
    ConfigLoaderConstructionTest() : TestFixture("ConfigLoader Construction")
    {
    }

    void test()
    {
        std::string logfile = "process_config";
        createJSON(logfile);

        // clang-format off
        json summary;
        testSchedulerConfig testconf_sched;
        summary["scheduler_config"] = {{"aging_threshold", testconf_sched.aging_threshold}, 
                                        {"max_priority", testconf_sched.max_priority}, 
                                        {"time_quantum", testconf_sched.time_quantum}};
        

        testProcessConfig testconf_proc;
        summary["processes"] = json::array({{{"pid", testconf_proc.pid},
                                     {"priority", testconf_proc.priority},
                                     {"burst_time", testconf_proc.burst},
                                     {"io_bound", testconf_proc.io_bound},
                                     {"io_interval", testconf_proc.io_interval}}});
        // clang-format on

        appendToJSON_object(logfile, summary);

        trackFile(logfile);

        assert_true(fileExists(logfile), "Logfile should exist");
        assert_true(countLines(logfile) > 0, "Logfile must have content");

        // need to have created and asserted, that the logfile exist (with content) before the configloader can load it.
        ConfigLoader cf(logfile);
    }
};

class ConfigLoaderGetProcessConfig : public TestFixture
{
   public:
    ConfigLoaderGetProcessConfig() : TestFixture("Get Process Config test")
    {
    }

    void test()
    {
        std::string logfile = "process_config";
        createJSON(logfile);

        // clang-format off
        json summary;
        testSchedulerConfig testconf_sched;
        summary["scheduler_config"] = {{"aging_threshold", testconf_sched.aging_threshold}, 
                                        {"max_priority", testconf_sched.max_priority}, 
                                        {"time_quantum", testconf_sched.time_quantum}};
        

        testProcessConfig testconf_proc;
        summary["processes"] = json::array({{{"pid", testconf_proc.pid},
                                     {"priority", testconf_proc.priority},
                                     {"burst_time", testconf_proc.burst},
                                     {"io_bound", testconf_proc.io_bound},
                                     {"io_interval", testconf_proc.io_interval}}});
        // clang-format on

        appendToJSON_object(logfile, summary);

        trackFile(logfile);

        assert_true(fileExists(logfile), "Logfile should exist");
        assert_true(countLines(logfile) > 0, "Logfile must have content");

        // need to have created and asserted, that the logfile exist (with content) before the configloader can load it.
        ConfigLoader cf(logfile);

        std::vector<ProcessConfig> getProcesses = cf.getProcessConfig();

        assert_true(!getProcesses.empty(), "Process config should not be empty");
        assert_equal(testconf_proc.pid, getProcesses.at(0).pid, "PID's should be equal");
        assert_equal(testconf_proc.burst, getProcesses.at(0).burst, "burst should be equal");
        assert_equal(testconf_proc.io_bound, getProcesses.at(0).io_bound, "io bound should be equal");
        assert_equal(testconf_proc.io_interval, getProcesses.at(0).io_interval, "io interval should be equal");
        assert_equal(testconf_proc.priority, getProcesses.at(0).priority, "priority should be equal");
    }
};

class ConfigLoaderGetSchedulerConfig : public TestFixture
{
   public:
    ConfigLoaderGetSchedulerConfig() : TestFixture("Get Scheduler Config test")
    {
    }

    void test()
    {
        std::string logfile = "process_config";
        createJSON(logfile);

        // clang-format off
        json summary;
        testSchedulerConfig testconf_sched;
        summary["scheduler_config"] = {{"aging_threshold", testconf_sched.aging_threshold}, 
                                        {"max_priority", testconf_sched.max_priority}, 
                                        {"time_quantum", testconf_sched.time_quantum}};
        

        testProcessConfig testconf_proc;
        summary["processes"] = json::array({{{"pid", testconf_proc.pid},
                                     {"priority", testconf_proc.priority},
                                     {"burst_time", testconf_proc.burst},
                                     {"io_bound", testconf_proc.io_bound},
                                     {"io_interval", testconf_proc.io_interval}}});
        // clang-format on

        appendToJSON_object(logfile, summary);

        trackFile(logfile);

        assert_true(fileExists(logfile), "Logfile should exist");
        assert_true(countLines(logfile) > 0, "Logfile must have content");

        // need to have created and asserted, that the logfile exist (with content) before the configloader can load it.
        ConfigLoader cf(logfile);

        SchedulerConfig getScheduler = cf.getSchedulerConfig();
        assert_equal(testconf_sched.aging_threshold, getScheduler.aging_threshold, "Aging threshold should be equal");
        assert_equal(testconf_sched.max_priority, getScheduler.max_priority, "Max priority should be equal");
        assert_equal(testconf_sched.time_quantum, getScheduler.time_quantum, "Time quantum should be equal");
    }
};

void run_configloader_tests()
{
    std::cout << "\n==== Configloader Test ====\n";
    int passed = 0, failed = 0;

    try
    {
        ConfigLoaderConstructionTest test1;
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
        ConfigLoaderGetProcessConfig test2;
        test2.run([&]() { test2.test(); });
        passed++;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception raised in [PROCESS CONFIG TEST] with: " << e.what() << std::endl;
        failed++;
    }

    try
    {
        ConfigLoaderGetSchedulerConfig test3;
        test3.run([&]() { test3.test(); });
        passed++;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception raised in [SCHEDULER CONFIG TEST] with: " << e.what() << std::endl;
        failed++;
    }

    std::cout << "Configloader test passed with: " << passed << " passed , " << failed << " failed" << std::endl;
}