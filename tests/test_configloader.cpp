#include <LogsJson.h>

#include "ConfigLoader.h"
#include "TestFixture.h"

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

        struct testConfigScheduler
        {
            int aging_threshold = 5;
            int max_priority = 3;
            int time_quantum = 4;
        };

        struct testConfigProcess
        {
            int pid = 1;
            int priority = 2;
            int burst = 5;
            bool io_bound = true;
            int io_interval = 4;
        };

        // clang-format off
        json summary;
        testConfigScheduler testconf_sched;
        summary["scheduler_config"] = {{"aging_threshold", testconf_sched.aging_threshold}, 
                                        {"max_priority", testconf_sched.max_priority}, 
                                        {"time_quantum", testconf_sched.time_quantum}};

        testConfigProcess testconf_proc;
        summary["process_config"] = {{"pid", testconf_proc.pid},
                                     {"priority", testconf_proc.priority},
                                     {"burst_time", testconf_proc.burst},
                                     {"io_bound", testconf_proc.io_bound},
                                     {"io_interval", testconf_proc.io_interval}};
        // clang-format on

        appendToJSON(logfile, summary);

        trackFile(logfile);

        assert_true(fileExists(logfile), "Logfile should exist");

        // fails here because cf checks if file is created -> which it isnt.
        ConfigLoader cf(logfile);
        // this also checks if the file exists which it doesnt ! -> we need to create the file first, and then do these.
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

    std::cout << "Configloader test passed with: " << passed << " passed , " << failed << " failed" << std::endl;
}