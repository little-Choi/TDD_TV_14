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
