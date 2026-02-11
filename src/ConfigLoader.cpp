#include "ConfigLoader.h"

#include <fstream>

ConfigLoader::ConfigLoader(std::string& config_file) : config_file(config_file)
{
    loadFromFile();
    if (!validate())
    {
        throw std::runtime_error("Error with validating file: " + config_file);
    }
}

void ConfigLoader::loadFromFile()
{
    std::ifstream file(config_file);
    if (!file.is_open())
    {
        throw std::runtime_error("Error opening file: " + config_file);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    auto config_data = json::parse(buffer.str());
}

std::vector<ProcessConfig> ConfigLoader::getProcessConfig() const
{
    std::vector<ProcessConfig> vector_conf;
    auto sched = config_data["processes"];
    for (const auto& i : sched)
    {
        ProcessConfig proc;
        proc.pid = i["pid"];
        proc.priority = i["priority"];
        proc.burst = i["burst"];
        proc.io_bound = i["io_bound"];
        proc.io_interval = i["io_interval"];
        vector_conf.push_back(proc);
    }

    return vector_conf;
}

SchedulerConfig ConfigLoader::getSchedulerConfig() const
{
    SchedulerConfig conf;

    auto sched = config_data["scheduler_config"];
    conf.aging_threshold = sched["aging_threshold"];
    conf.max_priority = sched["max_priority"];
    conf.time_quantum = sched["time_quantum"];

    return conf;
}

bool ConfigLoader::validate() const
{
    // call validate schedulerconfig
    // call validate processconfig
}

void ConfigLoader::validateSchedulerConfig() const
{
    // validate the required numbers are inside the demands
}

void ConfigLoader::validateProcessConfig() const
{
    // validate no duplicate pids
}