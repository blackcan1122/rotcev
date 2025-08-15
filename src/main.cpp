#include "rotcev.hpp"
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::cout << "==================================" << std::endl;
    std::cout << "Custom Vector Implementation [Rotcev]" << std::endl;

    std::cout << "" << std::endl;
    std::cout << "Test 1 - std::string - one push_back" << std::endl;
    std::cout << "========" << std::endl;
    std::string test_string = std::string("HEY");
    // rotcev
    Rotcev<std::string> MyRotcev;
    auto start = std::chrono::high_resolution_clock::now();
    MyRotcev.push_back(test_string);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << (end-start).count() << " NanoSeconds" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;


    std::cout << "==================================" << std::endl;
    std::cout << "Std Implementation [Vector]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Test 1 - std::string - one push_back" << std::endl;
    std::cout << "========" << std::endl;
    // std::vector
    std::vector<std::string> MyOtherVec;
    auto start2 = std::chrono::high_resolution_clock::now();
    MyOtherVec.push_back(test_string);
    auto end2 = std::chrono::high_resolution_clock::now();
    std::cout << (end2-start2).count() << " NanoSeconds" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;


    std::cout << "==================================" << std::endl;
    std::cout << "Total Difference is: " << ((end-start) - (end2-start2)).count() << std::endl;
    std::cout << "==================================" << std::endl;

}