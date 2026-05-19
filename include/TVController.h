/**
 * Copyright 2020 by Samsung Electronics, Inc.,
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung.
 */

#ifndef TV_CONTROLLER_H
#define TV_CONTROLLER_H

#include "ChannelPolicy.h"
#include "DigitInputBuffer.h"
#include "Tuner.h"
#include "remoteKey.h"
#include <algorithm>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace tv {

inline int stepLinearChannel(int current, int delta) {
    const int next = current + delta;
    if (next > kMaxChannel) {
        return kMinChannel;
    }
    if (next < kMinChannel) {
        return kMaxChannel;
    }
    return next;
}

enum class NavStep { Up, Down };

inline int navStepDelta(NavStep step, std::size_t listSize) {
    return step == NavStep::Up ? 1 : static_cast<int>(listSize) - 1;
}

struct SearchScanState {
    std::string startCurrent;
    bool hasStartCurrent = false;
    std::string firstReturned;
    bool hasFirst = false;
};

inline bool shouldStopSearch(const SearchScanState& state,
                             const std::string& chStr,
                             std::size_t collectedCount) {
    if (chStr.empty()) {
        return true;
    }
    if (!state.hasFirst) {
        return false;
    }
    if (state.hasStartCurrent && chStr == state.startCurrent) {
        return true;
    }
    if (chStr == state.firstReturned && collectedCount > 1) {
        return true;
    }
    return false;
}

struct IChannelStepPolicy {
    virtual ~IChannelStepPolicy() = default;
    virtual int step(int current, int direction) const = 0;
};

struct LinearStepPolicy : IChannelStepPolicy {
    int step(int current, int direction) const override {
        return stepLinearChannel(current, direction);
    }
};

struct SearchListStepPolicy : IChannelStepPolicy {
    explicit SearchListStepPolicy(const std::vector<int>& searchResults)
        : searchResults_(searchResults) {}

    int step(int current, int direction) const override {
        if (searchResults_.empty()) {
            return current;
        }

        const NavStep navStep = direction > 0 ? NavStep::Up : NavStep::Down;
        const auto it = std::find(searchResults_.begin(), searchResults_.end(), current);
        if (it == searchResults_.end()) {
            return navStep == NavStep::Up ? searchResults_.front() : searchResults_.back();
        }

        const auto index = static_cast<std::size_t>(it - searchResults_.begin());
        const auto size = searchResults_.size();
        const int delta = navStepDelta(navStep, size);
        const auto nextIndex = (index + size + static_cast<std::size_t>(delta)) % size;
        return searchResults_[nextIndex];
    }

private:
    const std::vector<int>& searchResults_;
};

} // namespace tv

class TVController {
private:
    Tuner* tuner_;
    DigitInputBuffer digitBuffer_;
    std::set<int> favorites_;
    std::vector<int> searchResults_;
    std::optional<int> navigationBase_;
    tv::LinearStepPolicy linearPolicy_;

    static bool isDigitKey(remoteKey key) {
        return key >= remoteKey::KEY_0 && key <= remoteKey::KEY_9;
    }

    [[nodiscard]] int parseTunerChannel() const {
        return std::stoi(tuner_->getCurrentCH());
    }

    void setTunerCh(int channel) {
        digitBuffer_.clear();
        std::cout << "현재 설정하는 채널 : " << channel << std::endl;
        tuner_->setCH(std::to_string(channel));
    }

    void clearInputBuffer() {
        digitBuffer_.clear();
        navigationBase_.reset();
    }

    void commitBufferedChannel() {
        if (const auto ch = digitBuffer_.tryTakeChannel()) {
            setTunerCh(*ch);
        }
    }

    void handleDigit(remoteKey key) {
        digitBuffer_.append(key);
        if (digitBuffer_.readyToCommit()) {
            commitBufferedChannel();
        }
    }

    void handleOk() { commitBufferedChannel(); }

    void performSearch() {
        searchResults_.clear();
        navigationBase_.reset();

        tv::SearchScanState scan;
        scan.startCurrent = tuner_->getCurrentCH();
        scan.hasStartCurrent = !scan.startCurrent.empty();

        std::unordered_set<int> seen;

        while (true) {
            const std::string chStr = tuner_->seekCH();
            if (tv::shouldStopSearch(scan, chStr, searchResults_.size())) {
                break;
            }

            if (!scan.hasFirst) {
                scan.firstReturned = chStr;
                scan.hasFirst = true;
                const int ch = std::stoi(chStr);
                searchResults_.push_back(ch);
                seen.insert(ch);
                continue;
            }

            const int ch = std::stoi(chStr);
            if (seen.insert(ch).second) {
                searchResults_.push_back(ch);
            }
        }

        std::sort(searchResults_.begin(), searchResults_.end());
    }

    void toggleFavorite() {
        const int current = parseTunerChannel();
        if (const auto it = favorites_.find(current); it != favorites_.end()) {
            favorites_.erase(it);
        } else {
            favorites_.insert(current);
        }
    }

    void goToNextFavorite() {
        const int current = parseTunerChannel();
        if (favorites_.empty()) {
            return;
        }

        const auto it = favorites_.upper_bound(current);
        const int target = (it == favorites_.end()) ? *favorites_.begin() : *it;
        setTunerCh(target);
    }

    struct NavigationContext {
        int current;
        std::optional<int> base;
    };

    [[nodiscard]] NavigationContext resolveNavigationContext() const {
        if (navigationBase_) {
            return {*navigationBase_, navigationBase_};
        }
        return {parseTunerChannel(), std::nullopt};
    }

    void channelUp() {
        const int current = parseTunerChannel();
        navigationBase_ = current;

        const tv::SearchListStepPolicy searchPolicy(searchResults_);
        const tv::IChannelStepPolicy& policy =
            searchResults_.empty() ? static_cast<const tv::IChannelStepPolicy&>(linearPolicy_)
                                   : static_cast<const tv::IChannelStepPolicy&>(searchPolicy);
        setTunerCh(policy.step(current, +1));
    }

    void channelDown() {
        const NavigationContext ctx = resolveNavigationContext();
        navigationBase_.reset();

        const tv::SearchListStepPolicy searchPolicy(searchResults_);
        const tv::IChannelStepPolicy& policy =
            searchResults_.empty() ? static_cast<const tv::IChannelStepPolicy&>(linearPolicy_)
                                   : static_cast<const tv::IChannelStepPolicy&>(searchPolicy);
        setTunerCh(policy.step(ctx.current, -1));
    }

    using Handler = void (TVController::*)();

    static const std::unordered_map<remoteKey, Handler>& handlerTable() {
        static const std::unordered_map<remoteKey, Handler> table = {
            {remoteKey::KEY_UP, &TVController::channelUp},
            {remoteKey::KEY_DOWN, &TVController::channelDown},
            {remoteKey::KEY_SEARCH, &TVController::performSearch},
            {remoteKey::KEY_FAVORITE_ADD, &TVController::toggleFavorite},
            {remoteKey::KEY_FAVORITE_NEXT, &TVController::goToNextFavorite},
        };
        return table;
    }

    void dispatchNonDigitKey(remoteKey key) {
        const auto& table = handlerTable();
        const auto it = table.find(key);
        if (it != table.end()) {
            (this->*(it->second))();
        }
    }

public:
    explicit TVController(Tuner* tuner) : tuner_(tuner) {}

    void pushButton(remoteKey key) {
        if (isDigitKey(key)) {
            handleDigit(key);
            return;
        }

        if (key == remoteKey::KEY_OK) {
            handleOk();
            return;
        }

        if (!digitBuffer_.empty()) {
            clearInputBuffer();
        }

        dispatchNonDigitKey(key);
    }
};

#endif // TV_CONTROLLER_H
