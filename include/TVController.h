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

#include "Tuner.h"
#include "remoteKey.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

class TVController {
private:
    Tuner* tuner;
    std::string processingCH;
    std::set<int> favorites;
    std::vector<int> searchResults;
    int navigationBase;

    static bool isDigitKey(remoteKey key) {
        return key >= remoteKey::KEY_0 && key <= remoteKey::KEY_9;
    }

    int getCurrentChannel() const {
        return std::stoi(tuner->getCurrentCH());
    }

    void setTunerCh(int channel) {
        processingCH.clear();
        std::cout << "현재 설정하는 채널 : " << channel << std::endl;
        tuner->setCH(std::to_string(channel));
    }

    void clearInputBuffer() {
        processingCH.clear();
        navigationBase = -1;
    }

    void handleDigit(remoteKey key) {
        processingCH += to_string(key);
        if (processingCH.length() >= 2) {
            setTunerCh(std::stoi(processingCH));
        }
    }

    void handleOk() {
        if (!processingCH.empty()) {
            setTunerCh(std::stoi(processingCH));
        }
    }

    void performSearch() {
        searchResults.clear();
        navigationBase = -1;

        const std::string startCurrent = tuner->getCurrentCH();
        const bool hasStartCurrent = !startCurrent.empty();

        std::string firstReturned;
        bool hasFirst = false;

        while (true) {
            const std::string chStr = tuner->seekCH();
            if (chStr.empty()) {
                break;
            }

            if (!hasFirst) {
                firstReturned = chStr;
                hasFirst = true;
                searchResults.push_back(std::stoi(chStr));
                continue;
            }

            if (hasStartCurrent && chStr == startCurrent) {
                break;
            }

            if (chStr == firstReturned && searchResults.size() > 1) {
                break;
            }

            const int ch = std::stoi(chStr);
            if (std::find(searchResults.begin(), searchResults.end(), ch) == searchResults.end()) {
                searchResults.push_back(ch);
            }
        }

        std::sort(searchResults.begin(), searchResults.end());
    }

    void toggleFavorite() {
        const int current = getCurrentChannel();
        if (favorites.count(current) > 0) {
            favorites.erase(current);
        } else {
            favorites.insert(current);
        }
    }

    void goToNextFavorite() {
        const int current = getCurrentChannel();
        if (favorites.empty()) {
            return;
        }

        auto it = favorites.upper_bound(current);
        if (it == favorites.end()) {
            setTunerCh(*favorites.begin());
        } else {
            setTunerCh(*it);
        }
    }

    void navigateSearchList(int current, int direction) {
        if (searchResults.empty()) {
            return;
        }

        const auto it = std::find(searchResults.begin(), searchResults.end(), current);
        if (it == searchResults.end()) {
            if (direction > 0) {
                setTunerCh(searchResults.front());
            } else {
                setTunerCh(searchResults.back());
            }
            return;
        }

        const auto index = static_cast<std::size_t>(it - searchResults.begin());
        const auto size = searchResults.size();
        std::size_t nextIndex;
        if (direction > 0) {
            nextIndex = (index + 1) % size;
        } else {
            nextIndex = (index + size - 1) % size;
        }
        setTunerCh(searchResults[nextIndex]);
    }

    int resolveNavigationChannel() const {
        if (navigationBase >= 0) {
            return navigationBase;
        }
        return getCurrentChannel();
    }

    void channelUp() {
        const int current = getCurrentChannel();
        navigationBase = current;

        if (!searchResults.empty()) {
            navigateSearchList(current, 1);
        } else if (current == 99) {
            setTunerCh(0);
        } else {
            setTunerCh(current + 1);
        }
    }

    void channelDown() {
        const int current = resolveNavigationChannel();
        navigationBase = -1;

        if (!searchResults.empty()) {
            navigateSearchList(current, -1);
        } else if (current == 0) {
            setTunerCh(99);
        } else {
            setTunerCh(current - 1);
        }
    }

public:
    explicit TVController(Tuner* tuner)
        : tuner(tuner), processingCH(""), navigationBase(-1) {}

    void pushButton(remoteKey key) {
        if (isDigitKey(key)) {
            handleDigit(key);
            return;
        }

        if (key == remoteKey::KEY_OK) {
            handleOk();
            return;
        }

        if (!processingCH.empty()) {
            clearInputBuffer();
        }

        switch (key) {
            case remoteKey::KEY_UP:
                channelUp();
                break;
            case remoteKey::KEY_DOWN:
                channelDown();
                break;
            case remoteKey::KEY_SEARCH:
                performSearch();
                break;
            case remoteKey::KEY_FAVORITE_ADD:
                toggleFavorite();
                break;
            case remoteKey::KEY_FAVORITE_NEXT:
                goToNextFavorite();
                break;
            default:
                break;
        }
    }
};

#endif // TV_CONTROLLER_H
