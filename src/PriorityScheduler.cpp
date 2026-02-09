#include <thread>

#include "LogsJson.h"
#include "SchedulerClass.h"

int main()
{
    auto exe_path = std::filesystem::current_path();
    setLogDirectory(exe_path / "logs");

    Scheduler scheduler("proces_logs");

    scheduler.setDebugFlags(Scheduler::EXEC | Scheduler::WARNING);

    std::thread t1([&scheduler]() { scheduler.run(); });

    t1.join();

    return 0;
}