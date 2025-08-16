#include "logging_profiling.hpp"
#include <iostream>

ProfileLogging::ProfileLogging(std::string ScenarioName)
{
    std::cout << "=====================================================" << std::endl;
    std::cout << "[START_TEST_SCENARIO]: " << ScenarioName << std::endl;
    m_startTime = std::chrono::high_resolution_clock::now();
    m_scenarioName = ScenarioName;
}

ProfileLogging::~ProfileLogging()
{
    auto EndTime = std::chrono::high_resolution_clock::now();
    auto Duration = EndTime - m_startTime;
    std::cout << "[FINISHED_TEST_SCENARIO]: " << m_scenarioName << " in: " << Duration.count() << std::endl;
    std::cout << "=====================================================" << std::endl;
}
