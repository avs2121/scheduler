#include "IOManager.h"
#include "TestFixture.h"
/*
Test Enqueue

Test enkelte Query metoder

Test Utilites

Test Debug metoder

Test funktion sidst

*/

class IOEnqueueTest : public TestFixture
{
   public:
    IOEnqueueTest() : TestFixture("IO Manager Enqueue Test")
    {
    }

    void test()
    {
        // clang-format off
    std::array<PCB, N> process = {
        PCB(1, 1, 5, 1, 2),
        PCB(2, 1, 5, 1, 2), 
        PCB(3, 1, 5, 1, 2),
        PCB(4, 1, 5, 1, 2), 
        PCB(5, 1, 5, 1, 2),
        PCB(6, 1,   5, 1, 2),
    };
        // clang-format on

        IOManager iom(process);

        assert_true(iom.isEmpty(), " IO Queue should be empty");
        iom.enqueue(0);
        assert_true(iom.containsPID(1), "IO Queue should now contain process with PID = 1");
        assert_equal(iom.size(), 1, " IO Queue size should now be 2");

        iom.enqueue(1);
        assert_true(iom.containsPID(2), "IO Queue should now contain process with PID = 2");
        assert_equal(iom.size(), 2, " IO Queue size should now be 2");
    }
};

class IOUpdateTest : public TestFixture
{
   public:
    IOUpdateTest() : TestFixture("IO Manager Update Test")
    {
    }

    void test()
    {
        // clang-format off
    std::array<PCB, N> process = {
        PCB(1, 1, 5, 1, 2),
        PCB(2, 1, 5, 1, 2), 
        PCB(3, 1, 5, 1, 2),
        PCB(4, 1, 5, 1, 2), 
        PCB(5, 1, 5, 1, 2),
        PCB(6, 1, 5, 1, 2),
    };
        // clang-format on

        IOManager iom(process);

        assert_true(iom.isEmpty(), " IO Queue should be empty");

        process[0].setState(ProcessState::WAITING_IO);
        process[1].setState(ProcessState::WAITING_IO);
        process[2].setState(ProcessState::WAITING_IO);

        assert_true(process[0].getState() == ProcessState::WAITING_IO, " p1 should have state: WAITING IO");
        assert_true(process[1].getState() == ProcessState::WAITING_IO, " p2 should have state: WAITING IO");
        assert_true(process[2].getState() == ProcessState::WAITING_IO, " p3 should have state: WAITING IO");

        iom.enqueue(0);
        iom.enqueue(1);
        iom.enqueue(2);

        assert_equal(iom.size(), 3, " IO Queue should have size 3");

        iom.updateIO();

        const std::deque<size_t>& test_deque = iom.getQueue();
        assert_equal(test_deque.size(), iom.size(), " test deque must be equal to original deque size (3)");

        for (size_t idx : test_deque)
        {
            PCB& proc = process[idx];
            assert_true(iom.containsPID(proc.getPid()), " Original deque, should contain all of the test deques elements");
            assert_equal(proc.getIORemainingTime(), proc.getIOInterval(), " IO Remaining and IO Interval should be the same");
        }
    }
};

class IOProcessTest : public TestFixture
{
   public:
    IOProcessTest() : TestFixture("IO Manager Process Test")
    {
    }

    void test()
    {
        // clang-format off
    std::array<PCB, N> process = {
        PCB(1, 1, 10, 1, 13),
        PCB(2, 1, 3, 1, 2), 
        PCB(3, 1, 3, 1, 2),
        PCB(4, 1, 5, 1, 2), 
        PCB(5, 1, 5, 1, 2),
        PCB(6, 1, 5, 1, 2),
    };
        // clang-format on

        IOManager iom(process);
        assert_true(iom.isEmpty(), " IO Queue should be empty");

        process[0].setState(ProcessState::WAITING_IO);
        process[1].setState(ProcessState::WAITING_IO);
        process[2].setState(ProcessState::WAITING_IO);

        assert_true(process[0].getState() == ProcessState::WAITING_IO, " p1 should have state: WAITING IO");
        assert_true(process[1].getState() == ProcessState::WAITING_IO, " p2 should have state: WAITING IO");
        assert_true(process[2].getState() == ProcessState::WAITING_IO, " p3 should have state: WAITING IO");

        iom.enqueue(0);
        iom.enqueue(1);
        iom.enqueue(2);

        assert_equal(iom.size(), 3, " IO Queue should have size 3");

        iom.updateIO();

        iom.processIO(TIME_QUANTUM);
        const std::vector<size_t>& finished_processes_test = iom.getFinishedProcesses();

        assert_equal(finished_processes_test.size(), 2, " Finished process size should be 2");
        assert_true(iom.containsPID(1), " IO Queue should only contain process with pid 1");

        for (size_t idx : finished_processes_test)
        {
            PCB& proc = process[idx];
            assert_equal(proc.getIORemainingTime(), 0, " Process' remaining IO Time should be 0");
            assert_true(proc.getState() == ProcessState::READY, " Process' process state should be READY");
        }

        assert_equal(iom.size(), 1, " IOM should now be 1, after 2 processes ended");
        assert_equal(iom.getMinRemainingIOTime(), 9, " Last process should have 9 (13-4) for remaining IO Time");

        iom.clear();
        assert_true(iom.isEmpty(), " IOM should be empty, after clear()");
    }
};

void run_io_tests()
{
    std::cout << "\n==== IO Manager Test ====\n";
    int passed = 0, failed = 0;

    try
    {
        IOEnqueueTest test1;
        test1.run([&]() { test1.test(); });
        passed++;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception raised in [IO ENQUEUE TEST] with: " << e.what() << std::endl;
        failed++;
    }

    try
    {
        IOUpdateTest test2;
        test2.run([&]() { test2.test(); });
        passed++;
    }

    catch (std::exception& e)
    {
        std::cout << "Exception raised in [IO UPDATE TEST] with: " << e.what() << std::endl;
        failed++;
    }

    try
    {
        IOProcessTest test3;
        test3.run([&]() { test3.test(); });
        passed++;
    }

    catch (std::exception& e)
    {
        std::cout << "Exception raised in [IO PROCESS TEST] with: " << e.what() << std::endl;
        failed++;
    }

    std::cout << "IO test passed with: " << passed << " passed , " << failed << " failed" << std::endl;
}