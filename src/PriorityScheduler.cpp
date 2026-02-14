#include <thread>

#include "LogsJson.h"
#include "SchedulerClass.h"

int main()
{
    auto exe_path = std::filesystem::current_path();
    setLogDirectory(exe_path / "logs");

    // make use of the /= operator, for portability to other platforms.
    auto config_path = std::filesystem::current_path() / "config" / "process_config.json";

    if (!std::filesystem::exists(config_path))
    {
        throw std::runtime_error("Config file doesnt exist at the provided path: " + config_path.string());
    }

    Scheduler scheduler("proces_logs", config_path.string());

    scheduler.setDebugFlags(Scheduler::EXEC | Scheduler::WARNING);

    std::thread t1([&scheduler]() { scheduler.run(); });

    t1.join();

    return 0;
}