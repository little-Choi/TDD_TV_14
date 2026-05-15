#include "TVController.h"
#include "Tuner.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

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

TEST_F(TVControllerTest,
       PushThreeDigitsUsesLastDigitAsFirstDigitOfNextChannel) {
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

TEST_F(TVControllerTest, PushFavoriteAddStoresCurrentChannelAsFavorite) {
  EXPECT_CALL(tuner, getCurrentCH())
      .WillOnce(Return("6"))
      .WillOnce(Return("0"));
  EXPECT_CALL(tuner, setCH("6"));

  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_NEXT_FAVORITE);
}

TEST_F(TVControllerTest,
       PushFavoriteAddRemovesCurrentChannelIfAlreadyFavorite) {
  EXPECT_CALL(tuner, getCurrentCH())
      .WillOnce(Return("6"))
      .WillOnce(Return("6"));
  EXPECT_CALL(tuner, setCH(_)).Times(0);

  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_NEXT_FAVORITE);
}

TEST_F(TVControllerTest,
       PushNextFavoriteChangesToSmallestFavoriteAboveCurrent) {
  InSequence sequence;
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("1"));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("4"));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("12"));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("56"));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("6"));
  EXPECT_CALL(tuner, setCH("12"));

  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_NEXT_FAVORITE);
}

TEST_F(TVControllerTest, PushNextFavoriteWrapsToSmallestFavorite) {
  InSequence sequence;
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("1"));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("4"));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("12"));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("56"));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("56"));
  EXPECT_CALL(tuner, setCH("1"));

  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_FAVORITE_ADD);
  controller.pushButton(remoteKey::KEY_NEXT_FAVORITE);
}

TEST_F(TVControllerTest, PushChannelUpWithoutSearchedChannelsIncrementsChannel) {
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("6"));
  EXPECT_CALL(tuner, setCH("7"));

  controller.pushButton(remoteKey::KEY_CHANNEL_UP);
}

TEST_F(TVControllerTest, PushChannelDownWithoutSearchedChannelsDecrementsChannel) {
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("6"));
  EXPECT_CALL(tuner, setCH("5"));

  controller.pushButton(remoteKey::KEY_CHANNEL_DOWN);
}

TEST_F(TVControllerTest, PushChannelUpWithoutSearchedChannelsWrapsToZero) {
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("99"));
  EXPECT_CALL(tuner, setCH("0"));

  controller.pushButton(remoteKey::KEY_CHANNEL_UP);
}

TEST_F(TVControllerTest, PushChannelDownWithoutSearchedChannelsWrapsToNinetyNine) {
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("0"));
  EXPECT_CALL(tuner, setCH("99"));

  controller.pushButton(remoteKey::KEY_CHANNEL_DOWN);
}

TEST_F(TVControllerTest, PushChannelUpWithSearchedChannelsMovesToNextStored) {
  InSequence sequence;
  EXPECT_CALL(tuner, seekCH())
      .WillOnce(Return("4"))
      .WillOnce(Return("6"))
      .WillOnce(Return("14"))
      .WillOnce(Return(""));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("6"));
  EXPECT_CALL(tuner, setCH("14"));

  controller.pushButton(remoteKey::KEY_CHANNEL_SEARCH);
  controller.pushButton(remoteKey::KEY_CHANNEL_UP);
}

TEST_F(TVControllerTest, PushChannelDownWithSearchedChannelsMovesToPreviousStored) {
  InSequence sequence;
  EXPECT_CALL(tuner, seekCH())
      .WillOnce(Return("4"))
      .WillOnce(Return("6"))
      .WillOnce(Return("14"))
      .WillOnce(Return(""));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("6"));
  EXPECT_CALL(tuner, setCH("4"));

  controller.pushButton(remoteKey::KEY_CHANNEL_SEARCH);
  controller.pushButton(remoteKey::KEY_CHANNEL_DOWN);
}

TEST_F(TVControllerTest, PushChannelUpWithSearchedChannelsWrapsToSmallest) {
  InSequence sequence;
  EXPECT_CALL(tuner, seekCH())
      .WillOnce(Return("4"))
      .WillOnce(Return("6"))
      .WillOnce(Return("14"))
      .WillOnce(Return(""));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("15"));
  EXPECT_CALL(tuner, setCH("4"));

  controller.pushButton(remoteKey::KEY_CHANNEL_SEARCH);
  controller.pushButton(remoteKey::KEY_CHANNEL_UP);
}

TEST_F(TVControllerTest, PushChannelDownWithSearchedChannelsWrapsToLargest) {
  InSequence sequence;
  EXPECT_CALL(tuner, seekCH())
      .WillOnce(Return("4"))
      .WillOnce(Return("6"))
      .WillOnce(Return("14"))
      .WillOnce(Return(""));
  EXPECT_CALL(tuner, getCurrentCH()).WillOnce(Return("15"));
  EXPECT_CALL(tuner, setCH("14"));

  controller.pushButton(remoteKey::KEY_CHANNEL_SEARCH);
  controller.pushButton(remoteKey::KEY_CHANNEL_DOWN);
}
