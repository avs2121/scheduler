#include <thread>

#include "LogsJson.h"
#include "SchedulerClass.h"

int main()
{
    auto exe_path = std::filesystem::current_path();
    setLogDirectory(exe_path / "logs");

    auto config_path = std::filesystem::current_path();
    std::string config_file;
    config_file += config_path.string() + "/config" + "/process_config.json";

    std::cout << config_file << std::endl;

    Scheduler scheduler("proces_logs", config_file);

    scheduler.setDebugFlags(Scheduler::EXEC | Scheduler::WARNING);

    std::thread t1([&scheduler]() { scheduler.run(); });

    t1.join();

    return 0;
}