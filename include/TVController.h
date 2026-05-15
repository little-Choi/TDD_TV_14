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
#include <set>
#include <string>

class TVController {
private:
    Tuner* tuner;
    std::string processingCH;
    std::set<int> favoriteChannels;
    std::set<int> searchedChannels;

    void setTunerCh();
    void toggleFavoriteChannel();
    void moveToNextFavoriteChannel();
    void searchChannels();
    void moveChannelUp();
    void moveChannelDown();

public:
    explicit TVController(Tuner* tuner);

    void pushButton(remoteKey key);
};

#endif // TV_CONTROLLER_H
