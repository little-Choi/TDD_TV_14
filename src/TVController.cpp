#include "TVController.h"

namespace {

constexpr int kMinChannel = 0;
constexpr int kMaxChannel = 99;
constexpr std::string::size_type kChannelInputDigits = 2;

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
  case remoteKey::KEY_CHANNEL_SEARCH:
  case remoteKey::KEY_CHANNEL_UP:
  case remoteKey::KEY_CHANNEL_DOWN:
    return false;
  }

  return false;
}

int toChannelNumber(const std::string &channel) { return std::stoi(channel); }

std::string toChannelText(int channel) { return std::to_string(channel); }

std::string normalizeChannel(const std::string &channel) {
  return toChannelText(toChannelNumber(channel));
}

} // namespace

TVController::TVController(Tuner *tuner) : tuner(tuner), processingCH("") {}

void TVController::pushButton(remoteKey key) {
  if (isNumberKey(key)) {
    handleNumberKey(key);
    return;
  }

  handleCommandKey(key);
}

bool TVController::handleCommandKey(remoteKey key) {
  if (key == remoteKey::KEY_OK) {
    setTunerCh();
    return true;
  }

  void (TVController::*command)() = nullptr;
  switch (key) {
  case remoteKey::KEY_FAVORITE_ADD:
    command = &TVController::toggleFavoriteChannel;
    break;
  case remoteKey::KEY_NEXT_FAVORITE:
    command = &TVController::moveToNextFavoriteChannel;
    break;
  case remoteKey::KEY_CHANNEL_SEARCH:
    command = &TVController::searchChannels;
    break;
  case remoteKey::KEY_CHANNEL_UP:
    command = &TVController::moveChannelUp;
    break;
  case remoteKey::KEY_CHANNEL_DOWN:
    command = &TVController::moveChannelDown;
    break;
  default:
    return false;
  }

  processingCH.clear();
  (this->*command)();
  return true;
}

void TVController::handleNumberKey(remoteKey key) {
  processingCH += to_string(key);

  if (processingCH.length() == kChannelInputDigits) {
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
  int currentChannel = toChannelNumber(tuner->getCurrentCH());
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

  int currentChannel = toChannelNumber(tuner->getCurrentCH());
  auto nextChannel = favoriteChannels.upper_bound(currentChannel);

  if (nextChannel == favoriteChannels.end()) {
    nextChannel = favoriteChannels.begin();
  }

  tuner->setCH(toChannelText(*nextChannel));
}

void TVController::searchChannels() {
  searchedChannels.clear();

  while (true) {
    std::string channel = tuner->seekCH();
    if (channel.empty()) {
      break;
    }

    searchedChannels.insert(toChannelNumber(channel));
  }
}

void TVController::moveChannelUp() {
  int currentChannel = toChannelNumber(tuner->getCurrentCH());

  if (searchedChannels.empty()) {
    int nextChannel =
        currentChannel == kMaxChannel ? kMinChannel : currentChannel + 1;
    tuner->setCH(toChannelText(nextChannel));
    return;
  }

  auto nextChannel = searchedChannels.upper_bound(currentChannel);
  if (nextChannel == searchedChannels.end()) {
    nextChannel = searchedChannels.begin();
  }

  tuner->setCH(toChannelText(*nextChannel));
}

void TVController::moveChannelDown() {
  int currentChannel = toChannelNumber(tuner->getCurrentCH());

  if (searchedChannels.empty()) {
    int previousChannel =
        currentChannel == kMinChannel ? kMaxChannel : currentChannel - 1;
    tuner->setCH(toChannelText(previousChannel));
    return;
  }

  auto previousChannel = searchedChannels.lower_bound(currentChannel);
  if (previousChannel == searchedChannels.begin()) {
    previousChannel = searchedChannels.end();
  }

  --previousChannel;
  tuner->setCH(toChannelText(*previousChannel));
}
