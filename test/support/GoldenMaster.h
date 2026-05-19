#ifndef GOLDEN_MASTER_H
#define GOLDEN_MASTER_H

#include <gtest/gtest.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

namespace golden {

inline bool shouldUpdateGolden() {
    const char* env = std::getenv("UPDATE_GOLDEN");
    return env != nullptr && env[0] != '\0' && env[0] != '0';
}

inline std::string fixtureRoot() {
#ifdef GOLDEN_FIXTURE_DIR
    return GOLDEN_FIXTURE_DIR;
#else
    return "test/golden/approved";
#endif
}

inline std::string approvedPath(const std::string& testName) {
    return fixtureRoot() + "/" + testName + ".approved.txt";
}

inline std::string receivedPath(const std::string& testName) {
    return fixtureRoot() + "/../received/" + testName + ".received.txt";
}

inline bool readFile(const std::string& path, std::string& out) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    out = ss.str();
    return true;
}

inline void writeFile(const std::string& path, const std::string& content) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out << content;
}

inline std::string normalizeNewlines(std::string text) {
    std::string normalized;
    normalized.reserve(text.size());
    for (char ch : text) {
        if (ch == '\r') {
            continue;
        }
        normalized.push_back(ch);
    }
    return normalized;
}

inline void approveOrCompare(const std::string& testName, const std::string& actual) {
    const std::string normalized = normalizeNewlines(actual);
    const std::string approvedFile = approvedPath(testName);

    if (shouldUpdateGolden()) {
        writeFile(approvedFile, normalized);
        return;
    }

    std::string expected;
    if (!readFile(approvedFile, expected)) {
        writeFile(receivedPath(testName), normalized);
        FAIL() << "Missing golden file: " << approvedFile
               << "\nRun with UPDATE_GOLDEN=1 to create it.";
    }

    expected = normalizeNewlines(expected);
    if (expected == normalized) {
        return;
    }

    writeFile(receivedPath(testName), normalized);
    FAIL() << "Golden master mismatch for " << testName
           << "\n  approved: " << approvedFile
           << "\n  received: " << receivedPath(testName)
           << "\nRe-run with UPDATE_GOLDEN=1 after reviewing the diff.";
}

} // namespace golden

#endif // GOLDEN_MASTER_H
