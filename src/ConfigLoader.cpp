#include "ConfigLoader.h"

#include <algorithm>
#include <fstream>
#include <set>

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
    validateSchedulerConfig();
    validateSchedulerConfig();

    return true;
}

// validate the required scheduler config numbers are inside the demands
void ConfigLoader::validateSchedulerConfig() const
{
    if (!config_data.contains("scheduler_config"))
    {
        throw std::runtime_error("Error in scheduler config - missing scheduler_config");
    }

    auto sched = config_data["scheduler_config"];
    if (sched["time_quantum"] <= 0)
    {
        throw std::runtime_error("Error in time_quantum in scheduler config");
    }

    if (sched["max_priority"] > 10 || sched["max_priority"] <= 0)
    {
        throw std::runtime_error("Error in max_priority in scheduler config");
    }

    if (sched["aging_threshold"] <= 0)
    {
        throw std::runtime_error("Error in aging_threshold in scheduler config");
    }
}

// validate process config for e.g no duplicate pids
void ConfigLoader::validateProcessConfig() const
{
    if (!config_data.contains("processes"))
    {
        throw std::runtime_error("Error in process config - missing process array");
    }

    if (!config_data.empty())
    {
        throw std::runtime_error("Error in process config - empty process array");
    }

    std::vector<ProcessConfig> vector_conf = getProcessConfig();
    auto sched = config_data["processes"];

    std::set<int> processSet;

    auto it = std::find_if(std::begin(vector_conf),
                           std::end(vector_conf),
                           [&processSet](const ProcessConfig& p)
                           {
                               if (processSet.find(p.pid) != processSet.end())
                                   return true;

                               else
                                   processSet.insert(p.pid);
                               return false;
                           });

    if (it != vector_conf.end())
    {
        throw std::runtime_error("Duplicate PID found: " + std::to_string(it->pid));
    }
}