#include <iostream>

void run_scheduler_tests();
void run_PCB_tests();
void run_io_tests();
void run_configloader_tests();

int main()
{
    std::cout << "=====================\n";
    std::cout << "= TEST SUITE RUNNER =\n";
    std::cout << "=====================\n";

    try
    {
        std::cout << "\n[1/4] Running Scheduler Test..." << std::endl;
        run_scheduler_tests();
        std::cout << "All test suites for scheduler completed succesfully" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "Test Suite failed: " << e.what() << std::endl;
    }

    try
    {
        std::cout << "\n[2/4] Running PCB Test..." << std::endl;
        run_PCB_tests();
        std::cout << "All test suites for PCB completed succesfully" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "Test Suite failed: " << e.what() << std::endl;
    }

    try
    {
        std::cout << "\n[3/4] Running IO Manager Test..." << std::endl;
        run_io_tests();
        std::cout << "All test suites for IO Manager completed succesfully" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "Test Suite failed: " << e.what() << std::endl;
    }

    try
    {
        std::cout << "\n[4/4] Running Config loader Test..." << std::endl;
        run_configloader_tests();
        std::cout << "All test suites for Config loader completed succesfully" << std::endl;
        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << "Test Suite failed: " << e.what() << std::endl;
        return 1;
    }
}