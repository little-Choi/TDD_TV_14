#ifndef DIGIT_INPUT_BUFFER_H
#define DIGIT_INPUT_BUFFER_H

#include "ChannelPolicy.h"
#include "remoteKey.h"
#include <optional>
#include <string>

class DigitInputBuffer {
public:
    void append(remoteKey key) { digits_ += to_string(key); }

    [[nodiscard]] bool empty() const { return digits_.empty(); }

    [[nodiscard]] bool readyToCommit() const {
        return digits_.length() >= static_cast<std::size_t>(tv::kMaxDigitLength);
    }

    [[nodiscard]] std::optional<int> tryTakeChannel() {
        if (digits_.empty()) {
            return std::nullopt;
        }
        const int channel = std::stoi(digits_);
        digits_.clear();
        return channel;
    }

    void clear() { digits_.clear(); }

private:
    std::string digits_;
};

#endif // DIGIT_INPUT_BUFFER_H
