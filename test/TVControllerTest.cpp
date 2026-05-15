#include "TVController.h"
#include "Tuner.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::InSequence;

class MockTunerForController : public Tuner {
public:
  MOCK_METHOD(std::string, seekCH, (), (override));
  MOCK_METHOD(void, setCH, (const std::string &ch), (override));
  MOCK_METHOD(std::string, getCurrentCH, (), (override));
};

class TVControllerTest : public ::testing::Test {
protected:
  MockTunerForController tuner;
  TVController controller{&tuner};
};

TEST_F(TVControllerTest, PushOneAndOkChangesChannelToOne) {
  EXPECT_CALL(tuner, setCH("1"));

  controller.pushButton(remoteKey::KEY_1);
  controller.pushButton(remoteKey::KEY_OK);
}

TEST_F(TVControllerTest, PushOneAndTwoChangesChannelToTwelve) {
  EXPECT_CALL(tuner, setCH("12"));

  controller.pushButton(remoteKey::KEY_1);
  controller.pushButton(remoteKey::KEY_2);
}

TEST_F(TVControllerTest, PushFourDigitsChangesChannelByTwoDigitGroups) {
  InSequence sequence;
  EXPECT_CALL(tuner, setCH("12"));
  EXPECT_CALL(tuner, setCH("34"));

  controller.pushButton(remoteKey::KEY_1);
  controller.pushButton(remoteKey::KEY_2);
  controller.pushButton(remoteKey::KEY_3);
  controller.pushButton(remoteKey::KEY_4);
}

TEST_F(TVControllerTest, PushThreeDigitsKeepsLastDigitPendingUntilOk) {
  InSequence sequence;
  EXPECT_CALL(tuner, setCH("45"));
  EXPECT_CALL(tuner, setCH("6"));

  controller.pushButton(remoteKey::KEY_4);
  controller.pushButton(remoteKey::KEY_5);
  controller.pushButton(remoteKey::KEY_6);
  controller.pushButton(remoteKey::KEY_OK);
}

TEST_F(TVControllerTest, PushThreeDigitsUsesLastDigitAsFirstDigitOfNextChannel) {
  InSequence sequence;
  EXPECT_CALL(tuner, setCH("45"));
  EXPECT_CALL(tuner, setCH("67"));

  controller.pushButton(remoteKey::KEY_4);
  controller.pushButton(remoteKey::KEY_5);
  controller.pushButton(remoteKey::KEY_6);
  controller.pushButton(remoteKey::KEY_7);
}

TEST_F(TVControllerTest, PushZeroAndSevenChangesChannelToSeven) {
  EXPECT_CALL(tuner, setCH("7"));

  controller.pushButton(remoteKey::KEY_0);
  controller.pushButton(remoteKey::KEY_7);
}
