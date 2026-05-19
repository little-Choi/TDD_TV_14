#ifndef FAKE_TUNER_H
#define FAKE_TUNER_H

#include "Tuner.h"
#include <string>
#include <vector>

// Deterministic Tuner for golden-master scenarios (no gmock).
class FakeTuner : public Tuner {
public:
    void setInitialChannel(int channel) { currentCh_ = std::to_string(channel); }

    void setSeekSequence(std::vector<std::string> channels) {
        seekSequence_ = std::move(channels);
        seekIndex_ = 0;
    }

    int currentChannel() const { return std::stoi(currentCh_); }

    std::string seekCH() override {
        if (seekIndex_ >= seekSequence_.size()) {
            return "";
        }
        const std::string ch = seekSequence_[seekIndex_++];
        currentCh_ = ch;
        return ch;
    }

    void setCH(const std::string& ch) override { currentCh_ = ch; }

    std::string getCurrentCH() override { return currentCh_; }

private:
    std::string currentCh_ = "0";
    std::vector<std::string> seekSequence_;
    std::size_t seekIndex_ = 0;
};

#endif // FAKE_TUNER_H
