/**
 * @file simple_test_framework.cpp
 * @brief Minimal test framework for TDD without external dependencies
 * @author AI Agent
 * @date 2025-11-06
 * 
 * Simple assertion-based test framework for rapid TDD development
 * when external testing frameworks are not available.
 */

#include "simple_test_framework.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace SimpleTest {

TestResults& TestResults::get_instance() {
    static TestResults instance;
    return instance;
}

void TestResults::add_test(const std::string& test_name, bool passed, const std::string& message) {
    tests_run_++;
    if (passed) {
        tests_passed_++;
        std::cout << "[PASS] " << test_name << std::endl;
    } else {
        tests_failed_++;
        std::cout << "[FAIL] " << test_name << " - " << message << std::endl;
    }
}

void TestResults::print_summary() {
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Tests Run: " << tests_run_ << std::endl;
    std::cout << "Passed: " << tests_passed_ << std::endl;
    std::cout << "Failed: " << tests_failed_ << std::endl;
    
    double pass_rate = tests_run_ > 0 ? (double)tests_passed_ / tests_run_ * 100.0 : 0.0;
    std::cout << "Pass Rate: " << std::fixed << std::setprecision(1) << pass_rate << "%" << std::endl;
    
    if (tests_failed_ == 0) {
        std::cout << "\n🎉 All tests PASSED!" << std::endl;
    } else {
        std::cout << "\n❌ " << tests_failed_ << " test(s) FAILED!" << std::endl;
    }
}

int TestResults::get_exit_code() {
    return tests_failed_ > 0 ? 1 : 0;
}

// Test assertion implementations
void assert_true_impl(bool condition, const std::string& test_name, const std::string& expression) {
    if (condition) {
        TestResults::get_instance().add_test(test_name, true, "");
    } else {
        std::ostringstream msg;
        msg << "Expected true, got false: " << expression;
        TestResults::get_instance().add_test(test_name, false, msg.str());
    }
}

void assert_false_impl(bool condition, const std::string& test_name, const std::string& expression) {
    if (!condition) {
        TestResults::get_instance().add_test(test_name, true, "");
    } else {
        std::ostringstream msg;
        msg << "Expected false, got true: " << expression;
        TestResults::get_instance().add_test(test_name, false, msg.str());
    }
}

void assert_eq_impl(uint32_t expected, uint32_t actual, const std::string& test_name, const std::string& expression) {
    if (expected == actual) {
        TestResults::get_instance().add_test(test_name, true, "");
    } else {
        std::ostringstream msg;
        msg << "Expected " << expected << ", got " << actual << ": " << expression;
        TestResults::get_instance().add_test(test_name, false, msg.str());
    }
}

void assert_ne_impl(uint32_t expected, uint32_t actual, const std::string& test_name, const std::string& expression) {
    if (expected != actual) {
        TestResults::get_instance().add_test(test_name, true, "");
    } else {
        std::ostringstream msg;
        msg << "Expected not " << expected << ", but got " << actual << ": " << expression;
        TestResults::get_instance().add_test(test_name, false, msg.str());
    }
}

void assert_lt_impl(double value, double limit, const std::string& test_name, const std::string& expression) {
    if (value < limit) {
        TestResults::get_instance().add_test(test_name, true, "");
    } else {
        std::ostringstream msg;
        msg << "Expected " << value << " < " << limit << ": " << expression;
        TestResults::get_instance().add_test(test_name, false, msg.str());
    }
}

} // namespace SimpleTest