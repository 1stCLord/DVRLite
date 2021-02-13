#include "Log.h"
#include "Web/Utils.hpp"
#include <fstream>
#include <iostream>
#include <chrono>
namespace DVRLite::Logger
{
    std::atomic<LogFilter> logFilter = LogFilter::DVRLite;
    std::ofstream logfile;

    void Init(const std::string& path)
    {
        logfile = std::ofstream(path);
    }

    void Filter(LogFilter filter)
    {
        logFilter = filter;
    }

    void Log(LogFilter filter, const std::string& logline)
    {
        if (Has(filter, logFilter))
        {
            std::string tsLogline = to_string(std::chrono::system_clock::now(), std::string(DATESTRINGFORMAT) + ": ") + logline;
            std::cout << tsLogline << std::endl;
            if (logfile.is_open())
                logfile << tsLogline << std::endl;
        }
    }
}