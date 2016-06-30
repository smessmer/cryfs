#pragma once
#ifndef MESSMER_CPPUTILS_TEST_LOGGING_TESTUTILS_LOGGINGTEST_H
#define MESSMER_CPPUTILS_TEST_LOGGING_TESTUTILS_LOGGINGTEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "cpp-utils/logging/logging.h"

class MockLogger final {
public:
    MockLogger():
            _capturedLogData(),
            _sink(std::make_shared<spdlog::sinks::ostream_sink<std::mutex>>(_capturedLogData, true)),
            _logger(spdlog::create("MockLogger", {_sink})) {
    }

    ~MockLogger() {
        spdlog::drop("MockLogger");
    };

    std::shared_ptr<spdlog::logger> get() {
        return _logger;
    }

    std::string capturedLog() const {
        return _capturedLogData.str();
    }
private:
    std::ostringstream _capturedLogData;
    std::shared_ptr<spdlog::sinks::ostream_sink<std::mutex>> _sink;
    std::shared_ptr<spdlog::logger> _logger;
};

class LoggingTest: public ::testing::Test {
public:
    LoggingTest(): mockLogger() {}

    std::string captureStderr(std::function<void()> func) {
        testing::internal::CaptureStderr();
        func();
        return testing::internal::GetCapturedStderr();
    }

    ~LoggingTest() {
        cpputils::logging::reset();
    }

    MockLogger mockLogger;
};

#endif
