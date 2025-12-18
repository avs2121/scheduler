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
    IOEnqueueTest() : TestFixture("IO Manager Enqueue Test") {}

    void test()
    {
        IOManager iom;
        auto p1 = std::make_unique<PCB>(1, 1, 5, true, 2); // on the heap.
        PCB *p1_ptr = p1.get();

        assert_true(iom.isEmpty(), " IO Queue should be empty");
        iom.enqueue(std::move(p1));
        assert_true(iom.contains(p1_ptr), "IO Queue should now contain p1");
        assert_equal(iom.size(), 1, " IO Queue size should now be 1");

        auto p2 = std::make_unique<PCB>(2, 1, 4, true, 5);
        PCB *p2_ptr = p2.get();
        iom.enqueue(std::move(p2));
        assert_true(iom.contains(p2_ptr), "IO Queue should now contain p2");
        assert_equal(iom.size(), 2, " IO Queue size should now be 2");
    }
};

class IOUpdateTest : public TestFixture
{
public:
    IOUpdateTest() : TestFixture("IO Manager Update Test") {}

    void test()
    {
        IOManager iom;
        auto p1 = std::make_unique<PCB>(1, 1, 5, true, 3);
        auto p2 = std::make_unique<PCB>(2, 1, 10, true, 4);
        auto p3 = std::make_unique<PCB>(3, 1, 7, true, 2);

        assert_true(iom.isEmpty(), " IO Queue should be empty");

        p1->setState(ProcessState::WAITING_IO);
        p2->setState(ProcessState::WAITING_IO);
        p3->setState(ProcessState::WAITING_IO);

        assert_true(p1->getState() == ProcessState::WAITING_IO, " p1 should have state: WAITING IO");
        assert_true(p2->getState() == ProcessState::WAITING_IO, " p2 should have state: WAITING IO");
        assert_true(p3->getState() == ProcessState::WAITING_IO, " p3 should have state: WAITING IO");

        iom.enqueue(std::move(p1));
        iom.enqueue(std::move(p2));
        iom.enqueue(std::move(p3));

        assert_equal(iom.size(), 3, " IO Queue should have size 3");

        iom.updateIO();

        const std::deque<std::unique_ptr<PCB>> &test_deque = iom.getQueue();
        assert_equal(test_deque.size(), iom.size(), " test deque must be equal to original deque size (3)");

        for (const auto &process : test_deque)
        {
            PCB *process_ptr = process.get();
            assert_true(iom.contains(process_ptr), " Original deque, should contain all of the test deques elements");
            assert_equal(process->getIORemainingTime(), process->getIOInterval(), " IO Remaining and IO Interval should be the same");
        }
    }
};

class IOProcessTest : public TestFixture
{
public:
    IOProcessTest() : TestFixture("IO Manager Process Test") {}

    void test()
    {
        IOManager iom;
        auto p1 = std::make_unique<PCB>(1, 1, 5, true, 2);
        auto p2 = std::make_unique<PCB>(2, 1, 5, true, 6);
        auto p3 = std::make_unique<PCB>(3, 1, 5, true, 3);

        assert_true(iom.isEmpty(), " IO Queue should be empty");

        p1->setState(ProcessState::WAITING_IO);
        p2->setState(ProcessState::WAITING_IO);
        p3->setState(ProcessState::WAITING_IO);

        assert_true(p1->getState() == ProcessState::WAITING_IO, " p1 should have state: WAITING IO");
        assert_true(p2->getState() == ProcessState::WAITING_IO, " p2 should have state: WAITING IO");
        assert_true(p3->getState() == ProcessState::WAITING_IO, " p3 should have state: WAITING IO");

        iom.enqueue(std::move(p1));
        iom.enqueue(std::move(p2));
        iom.enqueue(std::move(p3));

        assert_equal(iom.size(), 3, " IO Queue should have size 3");

        iom.updateIO();

        iom.processIO(TIME_QUANTUM);
        const std::vector<std::unique_ptr<PCB>> &finished_processes_test = iom.getFinishedProcesses();

        assert_equal(finished_processes_test.size(), 2, " Finished process size should be 2");
        assert_true(iom.containsPID(2), " IO Queue should only contain process with pid 2");

        for (const auto &process : finished_processes_test)
        {
            assert_equal(process->getIORemainingTime(), 0, " Process' remaining IO Time should be 0");
            assert_true(process->getState() == ProcessState::READY, " Process' process state should be READY");
        }

        assert_equal(iom.size(), 1, " IOM should now be 1, after 2 processes ended");
        assert_equal(iom.getMinRemainingIOTime(), 2, " Last process should have 2 for remaining IO Time");

        iom.clear();
        assert_true(iom.isEmpty(), " IOM should be empty, after clear()");
        */
    }
};

void run_io_tests()
{
    std::cout << "\n==== IO Manager Test ====\n";
    int passed = 0, failed = 0;

    try
    {
        IOEnqueueTest test1;
        test1.run([&]()
                  { test1.test(); });
        passed++;
    }
    catch (std::exception &e)
    {
        std::cout << "Exception raised in [IO ENQUEUE TEST] with: " << e.what() << std::endl;
        failed++;
    }

    try
    {
        IOUpdateTest test2;
        test2.run([&]()
                  { test2.test(); });
        passed++;
    }

    catch (std::exception &e)
    {
        std::cout << "Exception raised in [IO UPDATE TEST] with: " << e.what() << std::endl;
        failed++;
    }

    try
    {
        IOProcessTest test3;
        test3.run([&]()
                  { test3.test(); });
        passed++;
    }

    catch (std::exception &e)
    {
        std::cout << "Exception raised in [IO PROCESS TEST] with: " << e.what() << std::endl;
        failed++;
    }

    std::cout << "IO test passed with: " << passed << " passed , " << failed << " failed" << std::endl;
}