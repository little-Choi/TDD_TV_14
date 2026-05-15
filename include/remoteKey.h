/**
 * Copyright 2020 by Samsung Electronics, Inc.,
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung.
 */

#ifndef REMOTE_KEY_H
#define REMOTE_KEY_H

#include <string>

enum class remoteKey {
  KEY_0,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_OK,
  KEY_FAVORITE_ADD,
  KEY_NEXT_FAVORITE,
  KEY_CHANNEL_SEARCH,
  KEY_CHANNEL_UP,
  KEY_CHANNEL_DOWN
};

inline std::string to_string(remoteKey key) {
  switch (key) {
  case remoteKey::KEY_0:
    return "0";
  case remoteKey::KEY_1:
    return "1";
  case remoteKey::KEY_2:
    return "2";
  case remoteKey::KEY_3:
    return "3";
  case remoteKey::KEY_4:
    return "4";
  case remoteKey::KEY_5:
    return "5";
  case remoteKey::KEY_6:
    return "6";
  case remoteKey::KEY_7:
    return "7";
  case remoteKey::KEY_8:
    return "8";
  case remoteKey::KEY_9:
    return "9";
  case remoteKey::KEY_OK:
    return "OK";
  case remoteKey::KEY_FAVORITE_ADD:
    return "FAVORITE_ADD";
  case remoteKey::KEY_NEXT_FAVORITE:
    return "NEXT_FAVORITE";
  case remoteKey::KEY_CHANNEL_SEARCH:
    return "CHANNEL_SEARCH";
  case remoteKey::KEY_CHANNEL_UP:
    return "CHANNEL_UP";
  case remoteKey::KEY_CHANNEL_DOWN:
    return "CHANNEL_DOWN";
  }
  return "";
}

#endif // REMOTE_KEY_H
