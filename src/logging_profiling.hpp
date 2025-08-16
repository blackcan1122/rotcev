#pragma once
#include <string>
#include <chrono>


class ProfileLogging
{
public:
    ProfileLogging(std::string ScenarioName);
    ~ProfileLogging();

private:
std::string m_scenarioName;
std::chrono::high_resolution_clock::time_point m_startTime;

};