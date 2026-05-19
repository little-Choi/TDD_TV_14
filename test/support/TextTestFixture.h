#ifndef TEXT_TEST_FIXTURE_H
#define TEXT_TEST_FIXTURE_H

#include "GoldenMaster.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// TextTest-style fixture: capture cout + scripted key transcript, compare to .approved.txt
class TextTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        transcript_.clear();
        capture_.str("");
        capture_.clear();
        coutBuf_ = std::cout.rdbuf(capture_.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(coutBuf_);
    }

    void given(const std::string& line) { transcript_ << "# Given: " << line << '\n'; }

    void when(const std::string& line) { transcript_ << "# When: " << line << '\n'; }

    void then(const std::string& line) { transcript_ << "# Then: " << line << '\n'; }

    void logKey(const std::string& keyName) { transcript_ << "> " << keyName << '\n'; }

    void logLine(const std::string& line) { transcript_ << line << '\n'; }

    std::string buildTranscript() const {
        std::ostringstream full;
        full << transcript_.str();
        const std::string captured = capture_.str();
        if (!captured.empty()) {
            full << "--- stdout ---\n" << captured;
        }
        return full.str();
    }

    void approveGolden(const std::string& testName) {
        golden::approveOrCompare(testName, buildTranscript());
    }

private:
    std::ostringstream transcript_;
    std::stringstream capture_;
    std::streambuf* coutBuf_ = nullptr;
};

#endif // TEXT_TEST_FIXTURE_H
