#include "TVController.h"

namespace {

bool isNumberKey(remoteKey key) {
  switch (key) {
  case remoteKey::KEY_0:
  case remoteKey::KEY_1:
  case remoteKey::KEY_2:
  case remoteKey::KEY_3:
  case remoteKey::KEY_4:
  case remoteKey::KEY_5:
  case remoteKey::KEY_6:
  case remoteKey::KEY_7:
  case remoteKey::KEY_8:
  case remoteKey::KEY_9:
    return true;
  case remoteKey::KEY_OK:
  case remoteKey::KEY_FAVORITE_ADD:
  case remoteKey::KEY_NEXT_FAVORITE:
    return false;
  }

  return false;
}

std::string normalizeChannel(const std::string &channel) {
  return std::to_string(std::stoi(channel));
}

} // namespace

TVController::TVController(Tuner *tuner) : tuner(tuner), processingCH("") {}

void TVController::pushButton(remoteKey key) {
  if (key == remoteKey::KEY_OK) {
    setTunerCh();
    return;
  }

  if (key == remoteKey::KEY_FAVORITE_ADD) {
    processingCH.clear();
    toggleFavoriteChannel();
    return;
  }

  if (key == remoteKey::KEY_NEXT_FAVORITE) {
    processingCH.clear();
    moveToNextFavoriteChannel();
    return;
  }

  if (!isNumberKey(key)) {
    return;
  }

  processingCH += to_string(key);

  if (processingCH.length() == 2) {
    setTunerCh();
  }
}

void TVController::setTunerCh() {
  if (processingCH.empty()) {
    return;
  }

  tuner->setCH(normalizeChannel(processingCH));
  processingCH.clear();
}

void TVController::toggleFavoriteChannel() {
  int currentChannel = std::stoi(tuner->getCurrentCH());
  auto found = favoriteChannels.find(currentChannel);

  if (found == favoriteChannels.end()) {
    favoriteChannels.insert(currentChannel);
    return;
  }

  favoriteChannels.erase(found);
}

void TVController::moveToNextFavoriteChannel() {
  if (favoriteChannels.empty()) {
    return;
  }

  int currentChannel = std::stoi(tuner->getCurrentCH());
  auto nextChannel = favoriteChannels.upper_bound(currentChannel);

  if (nextChannel == favoriteChannels.end()) {
    nextChannel = favoriteChannels.begin();
  }

  tuner->setCH(std::to_string(*nextChannel));
}
