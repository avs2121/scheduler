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
        std::string logfile = "test_configloader.json";
        trackFile(logfile);

        // fails here because cf checks if file is created -> which it isnt.
        ConfigLoader cf(logfile);
        // this also checks if the file exists which it doesnt ! -> we need to create the file first, and then do these.
        assert_true(fileExists(logfile), "Logfile should exist");
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