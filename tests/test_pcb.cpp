#include "PCB.h"
#include "TestFixture.h"

const int TEST_AGING = 5;
const int TEST_TIME = 4;

class PCBCreationTest : public TestFixture
{
   public:
    PCBCreationTest() : TestFixture("PCB Creation Test")
    {
    }

    void test()
    {
        PCB p(1, 2, 10, true, 8, TEST_AGING, TEST_TIME);
        assert_equal(p.getPid(), 1, "Process should have PID = 1");
        assert_equal(p.getPriority(), 2, "Process should have prio = 2");
        assert_equal(p.getBurstTime(), 10, "Process should have burst time = 10");
        assert_true(p.isIOBound(), "Process should be IO bound");
        assert_equal(p.getIOInterval(), 8, "Process should have IO interval = 8");
    }
};

class PCBExecuteTest : public TestFixture
{
   public:
    PCBExecuteTest() : TestFixture("PCB Execute test")
    {
    }

    void test()
    {
        PCB p(1, 1, 10, false, 0, TEST_AGING, TEST_TIME);
        assert_equal(p.execute(TEST_TIME), 4, " Process [p] should execute one TIME QUANTUM");
        assert_equal(p.getRemainingTime(), 6, " Process [p] should have remaining time = 6");
        assert_equal(p.getCpuUsed(), 4, " Process [p] should have cpu used = 4");

        PCB p_io(2, 1, 5, true, 2, TEST_AGING, TEST_TIME);
        p_io.execute(TEST_TIME);
        assert_equal(p_io.getRemainingTime(), 3, " Process [p_io] should have remaining time = 3");
        assert_equal(p_io.getCpuUsed(), 2, " Process [p_io] should have cpu used = 2");

        PCB p_finish(3, 1, 2, false, 0, TEST_AGING, TEST_TIME);
        assert_equal(p_finish.execute(TEST_TIME), 2, " Process [p_finish] should finish with execute time = 2");
        assert_equal(p_finish.getRemainingTime(), 0, " Process [p_finish] should have remaining time = 0");
        assert_equal(p_finish.getCpuUsed(), 2, " Process [p_finish] should have cpu used = 2");
    }
};

class PCBAgeTest : public TestFixture
{
   public:
    PCBAgeTest() : TestFixture("PCB Age test")
    {
    }

    void test()
    {
        PCB p(1, 1, 10, false, 0, TEST_AGING, TEST_TIME);
        assert_true(!p.ageProcess(TEST_TIME), " Process [p] should age one TIME QUANTUM");
        assert_equal(p.getWaitingTime(), TEST_TIME, " Process [p] should have waited for one TIME QUANTUM");

        PCB p_upgrade(2, 2, 10, false, 0, TEST_AGING, TEST_TIME);
        assert_true(p_upgrade.ageProcess(5 * TEST_TIME),
                    " Process [p_upgrade] should wait for five TIME QUANTUM, and "
                    "upgrade prio");
        assert_equal(p_upgrade.getPriority(), 1, " Process [p_upgrade] should have priority = 1");
        assert_equal(p_upgrade.getWaitingTime(), 0, " Process' [p_upgrade] waiting time should have been reset to 0");

        PCB p_full_upgraded(3, 1, 10, false, 0, TEST_AGING, TEST_TIME);
        assert_true(!p_full_upgraded.ageProcess(5 * TEST_TIME),
                    " Process [p_full_upgraded] should wait for five TIME QUANTUM, "
                    "but not upgrade, because its prio = 1");
        assert_equal(p_full_upgraded.getPriority(), 1, " Process [p_full_upgraded] should still have priority = 1");
        assert_equal(p_full_upgraded.getWaitingTime(), 5 * TEST_TIME, " Process' [p_full_upgraded] waiting time should be five TIME QUANTUM");
    }
};

void run_PCB_tests()
{
    std::cout << "\n==== PCB Test ====\n";
    int passed = 0, failed = 0;

    try
    {
        PCBCreationTest test1;
        test1.run([&]() { test1.test(); });
        passed++;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception raised in [CREATION TEST] with: " << e.what() << std::endl;
        failed++;
    }

    try
    {
        PCBExecuteTest test2;
        test2.run([&]() { test2.test(); });
        passed++;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception raised in [EXECUTE TEST] with: " << e.what() << std::endl;
        failed++;
    }

    try
    {
        PCBAgeTest test3;
        test3.run([&]() { test3.test(); });
        passed++;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception raised in [AGE TEST] with: " << e.what() << std::endl;
        failed++;
    }
    std::cout << "PCB test passed with: " << passed << " passed , " << failed << " failed" << std::endl;
}