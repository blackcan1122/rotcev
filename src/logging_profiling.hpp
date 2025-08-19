#pragma once
#include "rotcev.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <map>

// Global variables to track performance statistics
struct PerformanceStats {
    int rotcev_wins = 0;
    int std_wins = 0;
    int ties = 0;
    long long total_rotcev_time = 0;
    long long total_std_time = 0;
    int total_tests = 0;
    
    // Separate tracking for operation types
    int push_back_rotcev_wins = 0;
    int push_back_std_wins = 0;
    int access_rotcev_wins = 0;
    int access_std_wins = 0;
    int push_back_tests = 0;
    int access_tests = 0;
    
    long long push_back_rotcev_total = 0;
    long long push_back_std_total = 0;
    long long access_rotcev_total = 0;
    long long access_std_total = 0;
    
    // Spike detection and filtering
    std::vector<std::pair<std::string, std::pair<long long, long long>>> spikes;
    std::vector<std::pair<std::string, std::pair<long long, long long>>> normal_results;
    long long spike_threshold = 3000; // ns - initial threshold, will be recalculated dynamically
    
    // Detailed operation tracking
    std::vector<std::pair<std::string, std::pair<long long, long long>>> all_results;
    
    // Dynamic spike detection
    void recalculateSpikes() {
        spikes.clear();
        normal_results.clear();
        
        if (all_results.empty()) return;
        
        // Calculate average time for both rotcev and std::vector
        long long total_rotcev = 0, total_std = 0;
        for (const auto& result : all_results) {
            total_rotcev += result.second.first;
            total_std += result.second.second;
        }
        
        double avg_rotcev = static_cast<double>(total_rotcev) / all_results.size();
        double avg_std = static_cast<double>(total_std) / all_results.size();
        double overall_avg = (avg_rotcev + avg_std) / 2.0;
        
        // Dynamic threshold: 2x the average time
        double dynamic_threshold = overall_avg * 10.0;
        spike_threshold = static_cast<long long>(dynamic_threshold);
        
        // Reclassify all results
        for (const auto& result : all_results) {
            bool is_spike = (result.second.first > dynamic_threshold || result.second.second > dynamic_threshold);
            if (is_spike) {
                spikes.push_back(result);
            } else {
                normal_results.push_back(result);
            }
        }
    }
    
    // Type-specific tracking
    std::map<std::string, int> type_rotcev_wins;
    std::map<std::string, int> type_std_wins;
    std::map<std::string, int> type_tests;
    std::map<std::string, long long> type_rotcev_total;
    std::map<std::string, long long> type_std_total;
} g_stats;

// Test class for non-trivial object testing
class TestObject {
private:
    std::string name;
    size_t size;
    int* data;

public:
    TestObject(const std::string& n = "default", size_t s = 100) 
        : name(n), size(s), data(new int[s]) {
        for(size_t i = 0; i < size; ++i) data[i] = static_cast<int>(i);
    }
    
    ~TestObject() { delete[] data; }
    
    // Copy constructor
    TestObject(const TestObject& other) 
        : name(other.name), size(other.size), data(new int[other.size]) {
        for(size_t i = 0; i < size; ++i) data[i] = other.data[i];
    }
    
    // Move constructor
    TestObject(TestObject&& other) noexcept
        : name(std::move(other.name)), size(other.size), data(other.data) {
        other.data = nullptr;
        other.size = 0;
    }
    
    // Copy assignment
    TestObject& operator=(const TestObject& other) {
        if(this != &other) {
            delete[] data;
            name = other.name;
            size = other.size;
            data = new int[size];
            for(size_t i = 0; i < size; ++i) data[i] = other.data[i];
        }
        return *this;
    }
    
    // Move assignment
    TestObject& operator=(TestObject&& other) noexcept {
        if(this != &other) {
            delete[] data;
            name = std::move(other.name);
            size = other.size;
            data = other.data;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
    
    const std::string& getName() const { return name; }
    size_t getSize() const { return size; }
};

// Utility functions for formatting
void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printSubHeader(const std::string& subtitle) {
    std::cout << "\n" << std::string(40, '-') << "\n";
    std::cout << "  " << subtitle << "\n";
    std::cout << std::string(40, '-') << "\n";
}

void printResult(const std::string& test_name, long long rotcev_time, long long vector_time, const std::string& type_name = "") {
    std::cout << std::left << std::setw(25) << test_name 
              << "| Rotcev: " << std::setw(8) << rotcev_time << "ns"
              << "| Std::vector: " << std::setw(8) << vector_time << "ns"
              << "| Diff: " << std::setw(8) << (rotcev_time - vector_time) << "ns";
    
    // Store all results for detailed analysis - spike detection will be done later
    g_stats.all_results.push_back({test_name, {rotcev_time, vector_time}});
    
    // Track statistics
    g_stats.total_tests++;
    g_stats.total_rotcev_time += rotcev_time;
    g_stats.total_std_time += vector_time;
    
    // Track by type if provided
    if (!type_name.empty()) {
        g_stats.type_tests[type_name]++;
        g_stats.type_rotcev_total[type_name] += rotcev_time;
        g_stats.type_std_total[type_name] += vector_time;
        if (rotcev_time < vector_time) g_stats.type_rotcev_wins[type_name]++;
        else if (vector_time < rotcev_time) g_stats.type_std_wins[type_name]++;
    }
    
    // Track by operation type
    if (test_name.find("push_back") != std::string::npos) {
        g_stats.push_back_tests++;
        g_stats.push_back_rotcev_total += rotcev_time;
        g_stats.push_back_std_total += vector_time;
        if (rotcev_time < vector_time) g_stats.push_back_rotcev_wins++;
        else if (vector_time < rotcev_time) g_stats.push_back_std_wins++;
    } else if (test_name.find("access") != std::string::npos) {
        g_stats.access_tests++;
        g_stats.access_rotcev_total += rotcev_time;
        g_stats.access_std_total += vector_time;
        if (rotcev_time < vector_time) g_stats.access_rotcev_wins++;
        else if (vector_time < rotcev_time) g_stats.access_std_wins++;
    }
    
    if (rotcev_time < vector_time) {
        double speedup = static_cast<double>(vector_time) / rotcev_time;
        std::cout << " (ROTCEV " << std::fixed << std::setprecision(2) << speedup << "x FASTER)";
        g_stats.rotcev_wins++;
    } else if (vector_time < rotcev_time) {
        double slowdown = static_cast<double>(rotcev_time) / vector_time;
        std::cout << " (ROTCEV " << std::fixed << std::setprecision(2) << slowdown << "x SLOWER)";
        g_stats.std_wins++;
    } else {
        std::cout << " (EQUAL)";
        g_stats.ties++;
    }
    
    std::cout << "\n";
}

// Template function for timing single operations
template<typename T, typename Container>
long long timeOperation(Container& container, T value, const std::string& operation) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (operation == "push_back") {
        container.push_back(value);
    } else if (operation == "access") {
        // For access, we need at least one element - check if we can access [0]
        auto ref = container[0]; // This will work if container has elements
        (void)ref; // suppress unused variable warning
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    return (end - start).count();
}

// Template function for bulk operations
template<typename T>
void testBulkOperations(const std::string& type_name, const std::vector<T>& test_data) {
    printSubHeader("Bulk Operations - " + type_name);
    
    // Test different bulk sizes
    std::vector<size_t> bulk_sizes = {10, 50, 100, 500, 2000};
    
    for (size_t bulk_size : bulk_sizes) {
        if (bulk_size > test_data.size()) continue;
        
        blck::rotcev<T> rotcev_container;
        std::vector<T> std_container;
        
        // Time bulk push_back operations
        auto start_rotcev = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < bulk_size; ++i) {
            T item = test_data[i % test_data.size()]; // Make a copy to avoid const issues
            rotcev_container.push_back(item);
        }
        auto end_rotcev = std::chrono::high_resolution_clock::now();
        
        auto start_std = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < bulk_size; ++i) {
            std_container.push_back(test_data[i % test_data.size()]);
        }
        auto end_std = std::chrono::high_resolution_clock::now();
        
        long long rotcev_time = (end_rotcev - start_rotcev).count();
        long long std_time = (end_std - start_std).count();
        
        std::stringstream ss;
        ss << bulk_size << " push_backs";
        printResult(ss.str(), rotcev_time, std_time, type_name);
        
        // Test bulk access operations
        if (bulk_size <= 100) { // Only for smaller sizes to avoid too much output
            start_rotcev = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < bulk_size; ++i) {
                auto ref = rotcev_container[i];
                (void)ref;
            }
            end_rotcev = std::chrono::high_resolution_clock::now();
            
            start_std = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < bulk_size; ++i) {
                auto ref = std_container[i];
                (void)ref;
            }
            end_std = std::chrono::high_resolution_clock::now();
            
            rotcev_time = (end_rotcev - start_rotcev).count();
            std_time = (end_std - start_std).count();
            
            ss.str("");
            ss << bulk_size << " accesses";
            printResult(ss.str(), rotcev_time, std_time, type_name);
        }
    }
}

int StartBenchmark() {
    printHeader("ROTCEV vs STD::VECTOR PERFORMANCE BENCHMARK");
    
    // Container size comparison
    std::cout << "\nContainer Sizes:\n";
    std::cout << "  blck::rotcev<std::string>: " << sizeof(blck::rotcev<std::string>) << " bytes\n";
    std::cout << "  std::vector<std::string>:  " << sizeof(std::vector<std::string>) << " bytes\n";
    
    // Test 1: Integer operations (trivial type)
    printSubHeader("INT OPERATIONS (Trivial Type)");
    {
        std::vector<int> test_ints = {42, 123, 999, 777, 555};
        
        blck::rotcev<int> rotcev_int;
        std::vector<int> std_int;
        
        // Single push_back tests
        for (size_t i = 0; i < 3; ++i) {
            int test_value = test_ints[i]; // Make a copy to avoid const issues
            long long rotcev_time = timeOperation(rotcev_int, test_value, "push_back");
            long long std_time = timeOperation(std_int, test_ints[i], "push_back");
            
            std::stringstream ss;
            ss << "push_back #" << (i + 1);
            printResult(ss.str(), rotcev_time, std_time, "int");
        }
        
        // Access tests
        for (size_t i = 0; i < 3 && i < 3; ++i) { // Limit to first 3 elements
            long long rotcev_time = timeOperation(rotcev_int, 0, "access");
            long long std_time = timeOperation(std_int, 0, "access");
            
            std::stringstream ss;
            ss << "access [" << i << "]";
            printResult(ss.str(), rotcev_time, std_time, "int");
        }
        
        testBulkOperations("int", test_ints);
    }
    
    // Test 2: String operations (non-trivial type)
    printSubHeader("STRING OPERATIONS (Non-Trivial Type)");
    {
        std::vector<std::string> test_strings = {
            "Hello", "World", "Performance", "Testing", "Benchmark",
            "This is a longer string for testing", "Short", "Medium length string"
        };
        
        blck::rotcev<std::string> rotcev_string;
        std::vector<std::string> std_string;
        
        // Single push_back tests
        for (size_t i = 0; i < 3; ++i) {
            std::string test_value = test_strings[i]; // Make a copy to avoid const issues
            long long rotcev_time = timeOperation(rotcev_string, test_value, "push_back");
            long long std_time = timeOperation(std_string, test_strings[i], "push_back");
            
            std::stringstream ss;
            ss << "push_back #" << (i + 1);
            printResult(ss.str(), rotcev_time, std_time, "std::string");
        }
        
        // Access tests
        for (size_t i = 0; i < 3 && i < 3; ++i) { // Limit to first 3 elements
            long long rotcev_time = timeOperation(rotcev_string, std::string(), "access");
            long long std_time = timeOperation(std_string, std::string(), "access");
            
            std::stringstream ss;
            ss << "access [" << i << "]";
            printResult(ss.str(), rotcev_time, std_time, "std::string");
        }
        
        testBulkOperations("std::string", test_strings);
    }
    
    // Test 3: Custom object operations (complex non-trivial type)
    printSubHeader("CUSTOM OBJECT OPERATIONS (Complex Non-Trivial Type)");
    {
        std::vector<TestObject> test_objects = {
            TestObject("Object1", 50),
            TestObject("Object2", 100),
            TestObject("Object3", 75),
            TestObject("LargeObject", 200),
            TestObject("LargeObject", 5000),
            TestObject("SmallObject", 25)
        };
        
        blck::rotcev<TestObject> rotcev_obj;
        std::vector<TestObject> std_obj;
        
        // Single push_back tests
        for (size_t i = 0; i < 3; ++i) {
            TestObject test_value = test_objects[i]; // Make a copy to avoid const issues
            long long rotcev_time = timeOperation(rotcev_obj, test_value, "push_back");
            long long std_time = timeOperation(std_obj, test_objects[i], "push_back");
            
            std::stringstream ss;
            ss << "push_back #" << (i + 1);
            printResult(ss.str(), rotcev_time, std_time, "TestObject");
        }
        
        // Access tests
        for (size_t i = 0; i < 3 && i < 3; ++i) { // Limit to first 3 elements
            long long rotcev_time = timeOperation(rotcev_obj, TestObject(), "access");
            long long std_time = timeOperation(std_obj, TestObject(), "access");
            
            std::stringstream ss;
            ss << "access [" << i << "]";
            printResult(ss.str(), rotcev_time, std_time, "TestObject");
        }
        
        testBulkOperations("TestObject", test_objects);
    }
    
    // Test 4: Double operations (trivial but larger type)
    printSubHeader("DOUBLE OPERATIONS (Larger Trivial Type)");
    {
        std::vector<double> test_doubles = {3.14159, 2.71828, 1.41421, 0.57721, 1.61803};
        
        blck::rotcev<double> rotcev_double;
        std::vector<double> std_double;
        
        // Single push_back tests
        for (size_t i = 0; i < 3; ++i) {
            double test_value = test_doubles[i]; // Make a copy to avoid const issues
            long long rotcev_time = timeOperation(rotcev_double, test_value, "push_back");
            long long std_time = timeOperation(std_double, test_doubles[i], "push_back");
            
            std::stringstream ss;
            ss << "push_back #" << (i + 1);
            printResult(ss.str(), rotcev_time, std_time, "double");
        }
        
        testBulkOperations("double", test_doubles);
    }
    
    // Test 5: Large Scale Stress Tests (1000+ elements)
    printSubHeader("LARGE SCALE STRESS TESTS (1000+ elements)");
    {
        // Generate large test datasets
        std::vector<int> large_ints;
        std::vector<std::string> large_strings;
        std::vector<double> large_doubles;
        
        for (int i = 0; i < 2000; ++i) {
            large_ints.push_back(i * 42 + 13);
            large_doubles.push_back(i * 3.14159 + 0.577);
            large_strings.push_back("String_" + std::to_string(i) + "_test_data");
        }
        
        // Test different large bulk sizes
        std::vector<size_t> large_bulk_sizes = {1000, 1500, 2000};
        
        for (size_t bulk_size : large_bulk_sizes) {
            std::cout << "\n--- Testing with " << bulk_size << " elements ---\n";
            
            // Large integer test
            {
                blck::rotcev<int> rotcev_large_int;
                std::vector<int> std_large_int;
                
                auto start_rotcev = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < bulk_size; ++i) {
                    rotcev_large_int.push_back(large_ints[i]);
                }
                auto end_rotcev = std::chrono::high_resolution_clock::now();
                
                auto start_std = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < bulk_size; ++i) {
                    std_large_int.push_back(large_ints[i]);
                }
                auto end_std = std::chrono::high_resolution_clock::now();
                
                long long rotcev_time = (end_rotcev - start_rotcev).count();
                long long std_time = (end_std - start_std).count();
                
                std::stringstream ss;
                ss << bulk_size << " int inserts";
                printResult(ss.str(), rotcev_time, std_time, "large_int");
                
                // Test random access on large container
                start_rotcev = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < 100; ++i) {
                    size_t idx = (i * 17 + 3) % bulk_size; // pseudo-random access
                    auto val = rotcev_large_int[idx];
                    (void)val;
                }
                end_rotcev = std::chrono::high_resolution_clock::now();
                
                start_std = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < 100; ++i) {
                    size_t idx = (i * 17 + 3) % bulk_size; // same pattern
                    auto val = std_large_int[idx];
                    (void)val;
                }
                end_std = std::chrono::high_resolution_clock::now();
                
                rotcev_time = (end_rotcev - start_rotcev).count();
                std_time = (end_std - start_std).count();
                
                ss.str("");
                ss << "100 random access (" << bulk_size << ")";
                printResult(ss.str(), rotcev_time, std_time, "large_int");
            }
            
            // Large string test  
            {
                blck::rotcev<std::string> rotcev_large_str;
                std::vector<std::string> std_large_str;
                
                auto start_rotcev = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < bulk_size; ++i) {
                    rotcev_large_str.push_back(large_strings[i]);
                }
                auto end_rotcev = std::chrono::high_resolution_clock::now();
                
                auto start_std = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < bulk_size; ++i) {
                    std_large_str.push_back(large_strings[i]);
                }
                auto end_std = std::chrono::high_resolution_clock::now();
                
                long long rotcev_time = (end_rotcev - start_rotcev).count();
                long long std_time = (end_std - start_std).count();
                
                std::stringstream ss;
                ss << bulk_size << " string inserts";
                printResult(ss.str(), rotcev_time, std_time, "large_string");
                
                // Test sequential access on large string container
                start_rotcev = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < 50; ++i) {
                    auto val = rotcev_large_str[i];
                    (void)val;
                }
                end_rotcev = std::chrono::high_resolution_clock::now();
                
                start_std = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < 50; ++i) {
                    auto val = std_large_str[i];
                    (void)val;
                }
                end_std = std::chrono::high_resolution_clock::now();
                
                rotcev_time = (end_rotcev - start_rotcev).count();
                std_time = (end_std - start_std).count();
                
                ss.str("");
                ss << "50 seq access (" << bulk_size << ")";
                printResult(ss.str(), rotcev_time, std_time, "large_string");
            }
            
            // Large double test (only for smaller sizes to avoid too much output)
            if (bulk_size <= 1500) {
                blck::rotcev<double> rotcev_large_dbl;
                std::vector<double> std_large_dbl;
                
                auto start_rotcev = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < bulk_size; ++i) {
                    rotcev_large_dbl.push_back(large_doubles[i]);
                }
                auto end_rotcev = std::chrono::high_resolution_clock::now();
                
                auto start_std = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < bulk_size; ++i) {
                    std_large_dbl.push_back(large_doubles[i]);
                }
                auto end_std = std::chrono::high_resolution_clock::now();
                
                long long rotcev_time = (end_rotcev - start_rotcev).count();
                long long std_time = (end_std - start_std).count();
                
                std::stringstream ss;
                ss << bulk_size << " double inserts";
                printResult(ss.str(), rotcev_time, std_time, "large_double");
            }
        }
        
        // Memory allocation pattern test
        std::cout << "\n--- Memory Growth Pattern Test ---\n";
        {
            blck::rotcev<int> growth_test;
            std::vector<int> std_growth_test;
            
            // Test how performance changes as container grows
            std::vector<size_t> growth_checkpoints = {100, 250, 500, 750, 1000, 1250, 1500};
            size_t rotcev_size = 0; // Track size manually since rotcev doesn't have size() method
            
            for (size_t checkpoint : growth_checkpoints) {
                // Fill up to checkpoint
                while (rotcev_size < checkpoint) {
                    growth_test.push_back(static_cast<int>(rotcev_size));
                    rotcev_size++;
                }
                while (std_growth_test.size() < checkpoint) {
                    std_growth_test.push_back(static_cast<int>(std_growth_test.size()));
                }
                
                // Test performance at this size
                auto start_rotcev = std::chrono::high_resolution_clock::now();
                for (int i = 0; i < 10; ++i) {
                    growth_test.push_back(checkpoint + i);
                    rotcev_size++;
                }
                auto end_rotcev = std::chrono::high_resolution_clock::now();
                
                auto start_std = std::chrono::high_resolution_clock::now();
                for (int i = 0; i < 10; ++i) {
                    std_growth_test.push_back(checkpoint + i);
                }
                auto end_std = std::chrono::high_resolution_clock::now();
                
                long long rotcev_time = (end_rotcev - start_rotcev).count();
                long long std_time = (end_std - start_std).count();
                
                std::stringstream ss;
                ss << "10 inserts @" << checkpoint << " size";
                printResult(ss.str(), rotcev_time, std_time, "growth_pattern");
            }
        }
    }
    
    printHeader("BENCHMARK COMPLETE");
    
    // Recalculate spike detection with dynamic threshold based on all collected data
    g_stats.recalculateSpikes();
    
    // Print comprehensive conclusion
    printHeader("PERFORMANCE ANALYSIS & CONCLUSION");
    
    std::cout << "\n📊 OVERALL STATISTICS:\n";
    std::cout << std::string(50, '-') << "\n";
    std::cout << "Total tests conducted: " << g_stats.total_tests << "\n";
    std::cout << "Rotcev wins: " << g_stats.rotcev_wins << " (" 
              << std::fixed << std::setprecision(1) 
              << (100.0 * g_stats.rotcev_wins / g_stats.total_tests) << "%)\n";
    std::cout << "Std::vector wins: " << g_stats.std_wins << " (" 
              << (100.0 * g_stats.std_wins / g_stats.total_tests) << "%)\n";
    std::cout << "Ties: " << g_stats.ties << " (" 
              << (100.0 * g_stats.ties / g_stats.total_tests) << "%)\n";
    
    // Spike Analysis
    std::cout << "\n⚡ DYNAMIC SPIKE ANALYSIS:\n";
    std::cout << std::string(50, '-') << "\n";
    
    // Calculate and display the dynamic threshold
    long long total_rotcev_all = 0, total_std_all = 0;
    for (const auto& result : g_stats.all_results) {
        total_rotcev_all += result.second.first;
        total_std_all += result.second.second;
    }
    double avg_rotcev_all = static_cast<double>(total_rotcev_all) / g_stats.all_results.size();
    double avg_std_all = static_cast<double>(total_std_all) / g_stats.all_results.size();
    double overall_avg_all = (avg_rotcev_all + avg_std_all) / 2.0;
    
    std::cout << "Average test time: " << std::fixed << std::setprecision(0) << overall_avg_all << "ns\n";
    std::cout << "Dynamic spike threshold: " << g_stats.spike_threshold << "ns (2x average)\n";
    std::cout << "Total spikes detected: " << g_stats.spikes.size() << " out of " << g_stats.all_results.size() << " tests\n";
    
    if (!g_stats.spikes.empty()) {
        std::cout << "Spike details:\n";
        for (const auto& spike : g_stats.spikes) {
            std::cout << "  " << std::left << std::setw(20) << spike.first 
                      << "| Rotcev: " << std::setw(8) << spike.second.first << "ns"
                      << "| Std::vector: " << std::setw(8) << spike.second.second << "ns";
            if (spike.second.first > spike.second.second) {
                double slowdown = static_cast<double>(spike.second.first) / spike.second.second;
                std::cout << " (Rotcev " << std::setprecision(2) << slowdown << "x slower)";
            } else {
                double speedup = static_cast<double>(spike.second.second) / spike.second.first;
                std::cout << " (Rotcev " << std::setprecision(2) << speedup << "x faster)";
            }
            std::cout << "\n";
        }
    }
    
    // Performance without spikes
    std::cout << "\n🎯 PERFORMANCE EXCLUDING SPIKES:\n";
    std::cout << std::string(50, '-') << "\n";
    if (!g_stats.normal_results.empty()) {
        long long normal_rotcev_total = 0, normal_std_total = 0;
        int normal_rotcev_wins = 0, normal_std_wins = 0;
        
        for (const auto& result : g_stats.normal_results) {
            normal_rotcev_total += result.second.first;
            normal_std_total += result.second.second;
            if (result.second.first < result.second.second) normal_rotcev_wins++;
            else if (result.second.second < result.second.first) normal_std_wins++;
        }
        
        double normal_avg_rotcev = static_cast<double>(normal_rotcev_total) / g_stats.normal_results.size();
        double normal_avg_std = static_cast<double>(normal_std_total) / g_stats.normal_results.size();
        
        std::cout << "Tests without spikes: " << g_stats.normal_results.size() << "\n";
        std::cout << "Rotcev wins: " << normal_rotcev_wins << " (" 
                  << (100.0 * normal_rotcev_wins / g_stats.normal_results.size()) << "%)\n";
        std::cout << "Average Rotcev time: " << std::setprecision(0) << normal_avg_rotcev << "ns\n";
        std::cout << "Average std::vector time: " << normal_avg_std << "ns\n";
        
        if (normal_avg_rotcev < normal_avg_std) {
            double speedup = normal_avg_std / normal_avg_rotcev;
            std::cout << "➡️  Without spikes, Rotcev is " << std::setprecision(2) << speedup << "x FASTER on average!\n";
        } else {
            double slowdown = normal_avg_rotcev / normal_avg_std;
            std::cout << "➡️  Without spikes, Rotcev is " << std::setprecision(2) << slowdown << "x slower on average\n";
        }
    }
    
    // Type-specific performance
    std::cout << "\n🏷️  TYPE-SPECIFIC PERFORMANCE:\n";
    std::cout << std::string(50, '-') << "\n";
    for (const auto& type_pair : g_stats.type_tests) {
        const std::string& type = type_pair.first;
        int tests = type_pair.second;
        int wins = g_stats.type_rotcev_wins[type];
        double avg_rotcev = static_cast<double>(g_stats.type_rotcev_total[type]) / tests;
        double avg_std = static_cast<double>(g_stats.type_std_total[type]) / tests;
        
        std::cout << type << ":\n";
        std::cout << "  Tests: " << tests << ", Wins: " << wins << " (" 
                  << (100.0 * wins / tests) << "%)\n";
        std::cout << "  Avg times: Rotcev " << std::setprecision(0) << avg_rotcev 
                  << "ns, std::vector " << avg_std << "ns\n";
        
        if (avg_rotcev < avg_std) {
            double speedup = avg_std / avg_rotcev;
            std::cout << "  Result: " << std::setprecision(2) << speedup << "x FASTER\n";
        } else {
            double slowdown = avg_rotcev / avg_std;
            std::cout << "  Result: " << std::setprecision(2) << slowdown << "x slower\n";
        }
        std::cout << "\n";
    }
    
    // Overall average performance
    double avg_rotcev = static_cast<double>(g_stats.total_rotcev_time) / g_stats.total_tests;
    double avg_std = static_cast<double>(g_stats.total_std_time) / g_stats.total_tests;
    std::cout << "\n🔢 AVERAGE PERFORMANCE:\n";
    std::cout << std::string(50, '-') << "\n";
    std::cout << "Average Rotcev time: " << std::fixed << std::setprecision(0) << avg_rotcev << "ns\n";
    std::cout << "Average std::vector time: " << avg_std << "ns\n";
    if (avg_rotcev < avg_std) {
        double speedup = avg_std / avg_rotcev;
        std::cout << "➡️  Rotcev is " << std::setprecision(2) << speedup << "x FASTER on average\n";
    } else {
        double slowdown = avg_rotcev / avg_std;
        std::cout << "➡️  Rotcev is " << std::setprecision(2) << slowdown << "x SLOWER on average\n";
    }
    
    // Best and worst performances
    std::cout << "\n🏆 BEST & WORST PERFORMANCES:\n";
    std::cout << std::string(50, '-') << "\n";
    
    // Find biggest wins and losses
    double best_speedup = 0.0;
    double worst_slowdown = 0.0;
    std::string best_test = "";
    std::string worst_test = "";
    
    for (const auto& result : g_stats.all_results) {
        double rotcev_time = static_cast<double>(result.second.first);
        double std_time = static_cast<double>(result.second.second);
        
        if (rotcev_time < std_time) {
            double speedup = std_time / rotcev_time;
            if (speedup > best_speedup) {
                best_speedup = speedup;
                best_test = result.first;
            }
        } else if (rotcev_time > std_time) {
            double slowdown = rotcev_time / std_time;
            if (slowdown > worst_slowdown) {
                worst_slowdown = slowdown;
                worst_test = result.first;
            }
        }
    }
    
    if (best_speedup > 0) {
        std::cout << "🥇 Best performance: " << best_test << " (" 
                  << std::setprecision(2) << best_speedup << "x faster)\n";
    }
    if (worst_slowdown > 0) {
        std::cout << "🥉 Worst performance: " << worst_test << " (" 
                  << std::setprecision(2) << worst_slowdown << "x slower)\n";
    }
    
    // Push_back specific analysis
    if (g_stats.push_back_tests > 0) {
        std::cout << "\n🚀 PUSH_BACK OPERATIONS:\n";
        std::cout << std::string(50, '-') << "\n";
        std::cout << "Total push_back tests: " << g_stats.push_back_tests << "\n";
        std::cout << "Rotcev wins: " << g_stats.push_back_rotcev_wins << " (" 
                  << (100.0 * g_stats.push_back_rotcev_wins / g_stats.push_back_tests) << "%)\n";
        std::cout << "Std::vector wins: " << g_stats.push_back_std_wins << " (" 
                  << (100.0 * g_stats.push_back_std_wins / g_stats.push_back_tests) << "%)\n";
        
        double avg_push_rotcev = static_cast<double>(g_stats.push_back_rotcev_total) / g_stats.push_back_tests;
        double avg_push_std = static_cast<double>(g_stats.push_back_std_total) / g_stats.push_back_tests;
        std::cout << "Average push_back time - Rotcev: " << std::setprecision(0) << avg_push_rotcev << "ns, ";
        std::cout << "Std::vector: " << avg_push_std << "ns\n";
        
        if (avg_push_rotcev < avg_push_std) {
            double speedup = avg_push_std / avg_push_rotcev;
            std::cout << "➡️  Rotcev push_back is " << std::setprecision(2) << speedup << "x FASTER on average\n";
        } else {
            double slowdown = avg_push_rotcev / avg_push_std;
            std::cout << "➡️  Rotcev push_back is " << std::setprecision(2) << slowdown << "x SLOWER on average\n";
        }
        
        // Explain the discrepancy if it exists
        bool wins_majority = g_stats.push_back_rotcev_wins > g_stats.push_back_std_wins;
        bool faster_average = avg_push_rotcev < avg_push_std;
        if (wins_majority != faster_average) {
            std::cout << "ℹ️  Note: Wins more individual tests but " << (faster_average ? "faster" : "slower") 
                      << " average suggests variable performance across different scenarios.\n";
        }
    }
    
    // Access specific analysis
    if (g_stats.access_tests > 0) {
        std::cout << "\n🎯 ACCESS OPERATIONS:\n";
        std::cout << std::string(50, '-') << "\n";
        std::cout << "Total access tests: " << g_stats.access_tests << "\n";
        std::cout << "Rotcev wins: " << g_stats.access_rotcev_wins << " (" 
                  << (100.0 * g_stats.access_rotcev_wins / g_stats.access_tests) << "%)\n";
        std::cout << "Std::vector wins: " << g_stats.access_std_wins << " (" 
                  << (100.0 * g_stats.access_std_wins / g_stats.access_tests) << "%)\n";
        
        double avg_access_rotcev = static_cast<double>(g_stats.access_rotcev_total) / g_stats.access_tests;
        double avg_access_std = static_cast<double>(g_stats.access_std_total) / g_stats.access_tests;
        std::cout << "Average access time - Rotcev: " << std::setprecision(0) << avg_access_rotcev << "ns, ";
        std::cout << "Std::vector: " << avg_access_std << "ns\n";
        
        if (avg_access_rotcev < avg_access_std) {
            double speedup = avg_access_std / avg_access_rotcev;
            std::cout << "➡️  Rotcev access is " << std::setprecision(2) << speedup << "x FASTER on average\n";
        } else {
            double slowdown = avg_access_rotcev / avg_access_std;
            std::cout << "➡️  Rotcev access is " << std::setprecision(2) << slowdown << "x SLOWER on average\n";
        }
        
        // Explain the discrepancy if it exists
        bool wins_majority = g_stats.access_rotcev_wins > g_stats.access_std_wins;
        bool faster_average = avg_access_rotcev < avg_access_std;
        if (wins_majority != faster_average) {
            std::cout << "ℹ️  Note: " << (wins_majority ? "Wins" : "Loses") << " more individual tests but " 
                      << (faster_average ? "faster" : "slower") 
                      << " average suggests inconsistent performance across test scenarios.\n";
        }
    }
    
    // Comprehensive comparison: WITH vs WITHOUT spikes
    std::cout << "\n📊 COMPREHENSIVE COMPARISON: WITH vs WITHOUT SPIKES:\n";
    std::cout << std::string(60, '=') << "\n";
    
    // Calculate metrics for normal results (without spikes)
    long long normal_rotcev_total = 0, normal_std_total = 0;
    int normal_rotcev_wins = 0, normal_std_wins = 0;
    
    if (!g_stats.normal_results.empty()) {
        for (const auto& result : g_stats.normal_results) {
            normal_rotcev_total += result.second.first;
            normal_std_total += result.second.second;
            if (result.second.first < result.second.second) normal_rotcev_wins++;
            else if (result.second.second < result.second.first) normal_std_wins++;
        }
    }
    
    double normal_avg_rotcev = g_stats.normal_results.empty() ? 0 : static_cast<double>(normal_rotcev_total) / g_stats.normal_results.size();
    double normal_avg_std = g_stats.normal_results.empty() ? 0 : static_cast<double>(normal_std_total) / g_stats.normal_results.size();
    
    // Print side-by-side comparison
    std::cout << std::left;
    std::cout << std::setw(35) << "METRIC" << "| " << std::setw(15) << "WITH SPIKES" << "| " << std::setw(15) << "WITHOUT SPIKES" << "| IMPACT\n";
    std::cout << std::string(80, '-') << "\n";
    
    // Tests count
    std::cout << std::setw(35) << "Total Tests" << "| " 
              << std::setw(15) << g_stats.total_tests << "| " 
              << std::setw(15) << g_stats.normal_results.size() << "| ";
    if (g_stats.spikes.size() > 0) {
        std::cout << g_stats.spikes.size() << " spikes removed\n";
    } else {
        std::cout << "No spikes\n";
    }
    
    // Win rates
    double overall_win_rate = 100.0 * g_stats.rotcev_wins / g_stats.total_tests;
    double normal_win_rate = g_stats.normal_results.empty() ? 0 : 100.0 * normal_rotcev_wins / g_stats.normal_results.size();
    std::cout << std::setw(35) << "Rotcev Win Rate" << "| " 
              << std::setw(13) << std::fixed << std::setprecision(1) << overall_win_rate << "%" << "| " 
              << std::setw(13) << normal_win_rate << "%" << "| ";
    if (normal_win_rate > overall_win_rate) {
        std::cout << "+" << (normal_win_rate - overall_win_rate) << "% improvement\n";
    } else {
        std::cout << (normal_win_rate - overall_win_rate) << "% change\n";
    }
    
    // Average times
    std::cout << std::setw(35) << "Avg Rotcev Time (ns)" << "| " 
              << std::setw(13) << std::setprecision(0) << avg_rotcev << " " << "| " 
              << std::setw(13) << normal_avg_rotcev << " " << "| ";
    if (normal_avg_rotcev < avg_rotcev && normal_avg_rotcev > 0) {
        double improvement = ((avg_rotcev - normal_avg_rotcev) / avg_rotcev) * 100;
        std::cout << std::setprecision(1) << improvement << "% faster\n";
    } else if (normal_avg_rotcev > 0) {
        double change = ((normal_avg_rotcev - avg_rotcev) / avg_rotcev) * 100;
        std::cout << std::setprecision(1) << change << "% slower\n";
    } else {
        std::cout << "N/A\n";
    }
    
    std::cout << std::setw(35) << "Avg Std::vector Time (ns)" << "| " 
              << std::setw(13) << std::setprecision(0) << avg_std << " " << "| " 
              << std::setw(13) << normal_avg_std << " " << "| ";
    if (normal_avg_std < avg_std && normal_avg_std > 0) {
        double improvement = ((avg_std - normal_avg_std) / avg_std) * 100;
        std::cout << std::setprecision(1) << improvement << "% faster\n";
    } else if (normal_avg_std > 0) {
        double change = ((normal_avg_std - avg_std) / avg_std) * 100;
        std::cout << std::setprecision(1) << change << "% slower\n";
    } else {
        std::cout << "N/A\n";
    }
    
    // Performance ratios
    std::cout << std::setw(35) << "Rotcev vs Std::vector" << "| ";
    if (avg_rotcev < avg_std) {
        double speedup = avg_std / avg_rotcev;
        std::cout << std::setw(13) << std::setprecision(2) << speedup << "x faster" << "| ";
    } else {
        double slowdown = avg_rotcev / avg_std;
        std::cout << std::setw(13) << slowdown << "x slower" << "| ";
    }
    
    if (normal_avg_rotcev > 0 && normal_avg_std > 0) {
        if (normal_avg_rotcev < normal_avg_std) {
            double normal_speedup = normal_avg_std / normal_avg_rotcev;
            std::cout << std::setw(13) << normal_speedup << "x faster" << "| ";
            if (avg_rotcev >= avg_std) {
                std::cout << "🎯 Spikes hide wins!\n";
            } else {
                double overall_speedup = avg_std / avg_rotcev;
                if (normal_speedup > overall_speedup) {
                    std::cout << "+" << (normal_speedup - overall_speedup) << "x improvement\n";
                } else {
                    std::cout << (normal_speedup - overall_speedup) << "x change\n";
                }
            }
        } else {
            double normal_slowdown = normal_avg_rotcev / normal_avg_std;
            std::cout << std::setw(13) << normal_slowdown << "x slower" << "| Same trend\n";
        }
    } else {
        std::cout << std::setw(13) << "N/A" << "| N/A\n";
    }
    
    std::cout << "\n🎯 KEY INSIGHTS:\n";
    std::cout << std::string(50, '-') << "\n";
    
    if (g_stats.spikes.size() > 0) {
        std::cout << "• " << g_stats.spikes.size() << " performance spikes detected (>" << g_stats.spike_threshold << "ns)\n";
        
        if (normal_avg_rotcev > 0 && normal_avg_std > 0) {
            bool overall_winning = avg_rotcev < avg_std;
            bool normal_winning = normal_avg_rotcev < normal_avg_std;
            
            if (normal_winning && !overall_winning) {
                std::cout << "• 🚨 SPIKES HIDE YOUR SUCCESS: Without spikes, you're actually winning!\n";
                double masked_advantage = (normal_avg_std / normal_avg_rotcev) - (avg_std / avg_rotcev);
                std::cout << "• 📈 Spikes mask " << std::setprecision(2) << masked_advantage << "x performance advantage\n";
            } else if (normal_winning && overall_winning) {
                double overall_speedup = avg_std / avg_rotcev;
                double normal_speedup = normal_avg_std / normal_avg_rotcev;
                if (normal_speedup > overall_speedup) {
                    std::cout << "• ⚡ Spikes reduce your advantage by " << (normal_speedup - overall_speedup) << "x\n";
                }
            } else if (!normal_winning && !overall_winning) {
                std::cout << "• 📊 Consistent performance pattern with and without spikes\n";
            }
        }
        
        if (normal_win_rate > overall_win_rate + 5) {
            std::cout << "• 🎯 Win rate improves significantly without spikes (+" 
                      << (normal_win_rate - overall_win_rate) << "%)\n";
        }
    } else {
        std::cout << "• ✅ No significant spikes detected - consistent performance!\n";
    }
    
    // Final verdict
    std::cout << "\n🏆 FINAL VERDICT:\n";
    std::cout << std::string(50, '=') << "\n";
    
    // Use average performance as the primary metric for final verdict
    bool rotcev_faster_overall = avg_rotcev < avg_std;
    bool rotcev_wins_more = g_stats.rotcev_wins > g_stats.std_wins;
    
    if (rotcev_faster_overall && rotcev_wins_more) {
        double speedup = avg_std / avg_rotcev;
        std::cout << "🎉 ROTCEV DOMINATES! (" << std::setprecision(1) 
                  << (100.0 * g_stats.rotcev_wins / g_stats.total_tests) << "% win rate, " 
                  << std::setprecision(2) << speedup << "x faster average)\n";
        std::cout << "Your implementation is both faster on average AND wins more individual tests!\n";
    } else if (rotcev_faster_overall) {
        double speedup = avg_std / avg_rotcev;
        std::cout << "🚀 ROTCEV WINS ON PERFORMANCE! (" << std::setprecision(2) << speedup << "x faster average)\n";
        std::cout << "Despite losing some individual tests, your average performance is superior!\n";
    } else if (rotcev_wins_more) {
        double win_percentage = 100.0 * g_stats.rotcev_wins / g_stats.total_tests;
        double slowdown = avg_rotcev / avg_std;
        std::cout << "⚖️  MIXED RESULTS (" << std::setprecision(1) << win_percentage << "% win rate, but " 
                  << std::setprecision(2) << slowdown << "x slower average)\n";
        std::cout << "You win more individual tests but have higher average latency.\n";
        std::cout << "💡 Suggests good performance in some scenarios, bottlenecks in others.\n";
    } else {
        double slowdown = avg_rotcev / avg_std;
        std::cout << "📈 STD::VECTOR WINS (" << std::setprecision(2) << slowdown << "x faster average, " 
                  << std::setprecision(1) << (100.0 * g_stats.std_wins / g_stats.total_tests) << "% win rate)\n";
        std::cout << "The standard library implementation outperformed your custom container.\n";
        std::cout << "💡 Consider optimizing memory allocation and capacity growth strategies!\n";
    }
    
    // Provide specific insights
    if (g_stats.push_back_rotcev_wins > g_stats.push_back_std_wins) {
        std::cout << "✅ Strong in push_back operations (" 
                  << (100.0 * g_stats.push_back_rotcev_wins / g_stats.push_back_tests) << "% win rate)\n";
    }
    if (g_stats.access_rotcev_wins > g_stats.access_std_wins) {
        std::cout << "✅ Strong in access operations (" 
                  << (100.0 * g_stats.access_rotcev_wins / g_stats.access_tests) << "% win rate)\n";
    }
    
    std::cout << "\nLegend:\n";
    std::cout << "  - Positive difference: Rotcev is slower\n";
    std::cout << "  - Negative difference: Rotcev is faster\n";
    std::cout << "  - Lower nanoseconds = better performance\n\n";
    
    return 0;
}