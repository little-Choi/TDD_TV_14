#include "TVController.h"
#include "Tuner.h"
#include <deque>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

std::string displayChannel(const std::string &channel) {
  return channel.empty() ? "<none>" : channel;
}

class ApprovalTuner : public Tuner {
public:
  ApprovalTuner(std::string initialChannel,
                const std::vector<std::string> &seekResults)
      : currentChannel(std::move(initialChannel)),
        seekResults(seekResults.begin(), seekResults.end()) {}

  std::string seekCH() override {
    std::string channel;
    if (!seekResults.empty()) {
      channel = seekResults.front();
      seekResults.pop_front();
    }

    calls.push_back("seekCH() -> " + displayChannel(channel));
    if (!channel.empty()) {
      currentChannel = channel;
    }
    return channel;
  }

  void setCH(const std::string &ch) override {
    calls.push_back("setCH(" + ch + ")");
    currentChannel = ch;
  }

  std::string getCurrentCH() override {
    calls.push_back("getCurrentCH() -> " + currentChannel);
    return currentChannel;
  }

  const std::vector<std::string> &getCalls() const { return calls; }

  const std::string &getCurrentChannel() const { return currentChannel; }

private:
  std::string currentChannel;
  std::deque<std::string> seekResults;
  std::vector<std::string> calls;
};

struct Scenario {
  Scenario(std::string name, std::vector<remoteKey> input)
      : name(std::move(name)), input(std::move(input)) {}

  Scenario(std::string name, std::vector<std::string> seekResults,
           std::vector<remoteKey> input)
      : name(std::move(name)), seekResults(std::move(seekResults)),
        input(std::move(input)) {}

  Scenario(std::string name, std::string initialChannel,
           std::vector<remoteKey> input)
      : name(std::move(name)), initialChannel(std::move(initialChannel)),
        input(std::move(input)) {}

  std::string name;
  std::string initialChannel = "0";
  std::vector<std::string> seekResults;
  std::vector<remoteKey> input;
};

std::string approvalDirectory() {
  std::string testFile = __FILE__;
  std::string::size_type slash = testFile.find_last_of("/\\");
  return testFile.substr(0, slash) + "/approval";
}

std::string readFile(const std::string &path) {
  std::ifstream file(path);
  std::ostringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void writeFile(const std::string &path, const std::string &content) {
  std::ofstream file(path);
  file << content;
}

std::string runScenario(const Scenario &scenario) {
  ApprovalTuner tuner(scenario.initialChannel, scenario.seekResults);
  TVController controller(&tuner);

  std::ostringstream output;
  output << "Scenario: " << scenario.name << "\n";
  output << "Initial channel: " << scenario.initialChannel << "\n";

  if (!scenario.seekResults.empty()) {
    output << "Seek results:";
    for (const std::string &channel : scenario.seekResults) {
      output << " " << displayChannel(channel);
    }
    output << "\n";
  }

  output << "Input:";
  for (remoteKey key : scenario.input) {
    output << " " << to_string(key);
  }
  output << "\n";

  for (remoteKey key : scenario.input) {
    controller.pushButton(key);
  }

  output << "Calls:\n";
  for (const std::string &call : tuner.getCalls()) {
    output << "  " << call << "\n";
  }

  output << "Final channel: " << tuner.getCurrentChannel() << "\n\n";
  return output.str();
}

std::string runApprovalScenarios() {
  const std::vector<Scenario> scenarios = {
      Scenario{"single digit confirmed",
               {remoteKey::KEY_1, remoteKey::KEY_OK}},
      Scenario{"two digit channel", {remoteKey::KEY_1, remoteKey::KEY_2}},
      Scenario{"four digits grouped by pairs",
               {remoteKey::KEY_1, remoteKey::KEY_2, remoteKey::KEY_3,
                remoteKey::KEY_4}},
      Scenario{"third digit remains pending until OK",
               {remoteKey::KEY_4, remoteKey::KEY_5, remoteKey::KEY_6,
                remoteKey::KEY_OK}},
      Scenario{"leading zero normalizes channel",
               {remoteKey::KEY_0, remoteKey::KEY_7}},
      Scenario{"favorite channels rotate to next higher channel",
               {remoteKey::KEY_0, remoteKey::KEY_1,
                remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_0,
                remoteKey::KEY_4, remoteKey::KEY_FAVORITE_ADD,
                remoteKey::KEY_1, remoteKey::KEY_2,
                remoteKey::KEY_FAVORITE_ADD, remoteKey::KEY_5,
                remoteKey::KEY_6, remoteKey::KEY_FAVORITE_ADD,
                remoteKey::KEY_0, remoteKey::KEY_6,
                remoteKey::KEY_NEXT_FAVORITE}},
      Scenario{"searched channels guide channel up",
               {"4", "6", "14", ""},
               {remoteKey::KEY_CHANNEL_SEARCH, remoteKey::KEY_0,
                remoteKey::KEY_6, remoteKey::KEY_CHANNEL_UP}},
      Scenario{"searched channels guide channel down",
               {"4", "6", "14", ""},
               {remoteKey::KEY_CHANNEL_SEARCH, remoteKey::KEY_1,
                remoteKey::KEY_5, remoteKey::KEY_CHANNEL_DOWN}},
      Scenario{"channel up wraps from max without search", "99",
               {remoteKey::KEY_CHANNEL_UP}},
      Scenario{"channel down wraps from zero without search",
               {remoteKey::KEY_CHANNEL_DOWN}},
  };

  std::ostringstream output;
  for (const Scenario &scenario : scenarios) {
    output << runScenario(scenario);
  }
  return output.str();
}

} // namespace

TEST(TVControllerApprovalTest, RemoteControlBehaviorMatchesApprovedOutput) {
  std::string actual = runApprovalScenarios();
  std::string approvedPath =
      approvalDirectory() + "/TVControllerApproval.approved.txt";
  std::string receivedPath =
      approvalDirectory() + "/TVControllerApproval.received.txt";
  std::string approved = readFile(approvedPath);

  if (approved != actual) {
    writeFile(receivedPath, actual);
  }

  EXPECT_EQ(approved, actual)
      << "Approval output changed. Received output written to "
      << receivedPath;
}
