#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Tuner.h"
#include "TVController.h"
#include <string>

class MockTunerForController : public Tuner {
public:
    MOCK_METHOD(std::string, seekCH, (), (override));
    MOCK_METHOD(void, setCH, (const std::string& ch), (override));
    MOCK_METHOD(std::string, getCurrentCH, (), (override));
};

class TVControllerTest : public ::testing::Test {
protected:
    MockTunerForController mockTuner;
    TVController controller{&mockTuner};

    static int toChannel(const std::string& ch) {
        return std::stoi(ch);
    }

    void givenCurrentChannel(int channel) {
        ON_CALL(mockTuner, getCurrentCH())
            .WillByDefault(::testing::Return(std::to_string(channel)));
    }

    void givenSearchResults(const std::vector<std::string>& channels) {
        ::testing::InSequence seq;
        for (const auto& ch : channels) {
            EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return(ch));
        }
        if (!channels.empty()) {
            EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return(channels.front()));
        }
        controller.pushButton(remoteKey::KEY_SEARCH);
    }

    void whenPush(remoteKey key) {
        controller.pushButton(key);
    }

    void thenExpectSetChannel(int expected) {
        EXPECT_CALL(mockTuner, setCH(::testing::_))
            .Times(1)
            .WillOnce(::testing::Invoke([expected](const std::string& ch) {
                ASSERT_EQ(expected, toChannel(ch));
            }));
    }
};

// ---------------------------------------------------------------------------
// README §1 — 숫자 버튼으로 채널 변경 (최소 5)
// ---------------------------------------------------------------------------

TEST_F(TVControllerTest, should_set_channel_1_when_digit_1_and_ok) {
    // Given: 초기 상태, 숫자 입력 버퍼 비어 있음
    ::testing::InSequence seq;
    thenExpectSetChannel(1);

    // When: '1', '확인'
    whenPush(remoteKey::KEY_1);
    whenPush(remoteKey::KEY_OK);
}

TEST_F(TVControllerTest, should_set_channel_12_when_digits_1_and_2) {
    // Given: 초기 상태
    thenExpectSetChannel(12);

    // When: '1', '2' (2자리 즉시 확정)
    whenPush(remoteKey::KEY_1);
    whenPush(remoteKey::KEY_2);
}

TEST_F(TVControllerTest, should_set_channels_12_then_34_when_four_digits) {
    // Given: 초기 상태
    ::testing::InSequence seq;
    thenExpectSetChannel(12);
    thenExpectSetChannel(34);

    // When: '1','2','3','4'
    whenPush(remoteKey::KEY_1);
    whenPush(remoteKey::KEY_2);
    whenPush(remoteKey::KEY_3);
    whenPush(remoteKey::KEY_4);
}

TEST_F(TVControllerTest, should_set_channels_45_then_6_when_digits_4_5_6_and_ok) {
    // Given: 초기 상태
    ::testing::InSequence seq;
    thenExpectSetChannel(45);
    thenExpectSetChannel(6);

    // When: '4','5','6', '확인'
    whenPush(remoteKey::KEY_4);
    whenPush(remoteKey::KEY_5);
    whenPush(remoteKey::KEY_6);
    whenPush(remoteKey::KEY_OK);
}

TEST_F(TVControllerTest, should_set_channel_7_when_digits_0_and_7) {
    // Given: 앞자리 0 포함 입력
    thenExpectSetChannel(7);

    // When: '0','7' → 7번 채널
    whenPush(remoteKey::KEY_0);
    whenPush(remoteKey::KEY_7);
}

TEST_F(TVControllerTest, should_set_channel_99_when_digits_9_and_9) {
    // Given: 경계값 최대 채널 99
    thenExpectSetChannel(99);

    // When: '9','9'
    whenPush(remoteKey::KEY_9);
    whenPush(remoteKey::KEY_9);
}

TEST_F(TVControllerTest, should_confirm_45_only_when_key_up_after_digits_4_5_6) {
    // Given: '4','5','6' 후 대기 중인 '6'
    ::testing::InSequence seq;
    thenExpectSetChannel(45);
    whenPush(remoteKey::KEY_4);
    whenPush(remoteKey::KEY_5);
    whenPush(remoteKey::KEY_6);

    // When: '6' 무효화 후 시청 중 6번에서 채널 업 → 7
    givenCurrentChannel(6);
    thenExpectSetChannel(7);
    whenPush(remoteKey::KEY_UP);
}

// ---------------------------------------------------------------------------
// README §2 — 선호 채널 추가 (최소 5)
// ---------------------------------------------------------------------------

TEST_F(TVControllerTest, should_add_favorite_when_current_channel_not_in_list) {
    // Given: 10번 시청 중, 선호 목록 비어 있음
    givenCurrentChannel(10);
    EXPECT_CALL(mockTuner, getCurrentCH()).WillRepeatedly(::testing::Return("10"));

    // When: 선호채널추가
    whenPush(remoteKey::KEY_FAVORITE_ADD);

    // Then: 다음선호채널 시 10번으로 이동
    givenCurrentChannel(5);
    thenExpectSetChannel(10);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);
}

TEST_F(TVControllerTest, should_remove_favorite_when_current_channel_already_in_list) {
    // Given: 10번 선호 등록
    givenCurrentChannel(10);
    EXPECT_CALL(mockTuner, getCurrentCH()).Times(::testing::AtLeast(1));
    whenPush(remoteKey::KEY_FAVORITE_ADD);

    // When/Then: 등록 시 5번→10번
    givenCurrentChannel(5);
    thenExpectSetChannel(10);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);

    // Given: 10번 선호 삭제(토글)
    givenCurrentChannel(10);
    whenPush(remoteKey::KEY_FAVORITE_ADD);

    // When/Then: 삭제 후 5번에서 다음선호 시 setCH 없음
    givenCurrentChannel(5);
    EXPECT_CALL(mockTuner, getCurrentCH()).Times(::testing::AtLeast(1));
    EXPECT_CALL(mockTuner, setCH(::testing::_)).Times(0);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);
}

TEST_F(TVControllerTest, should_toggle_favorite_at_channel_0) {
    // Given: 경계값 0번 채널
    givenCurrentChannel(0);
    whenPush(remoteKey::KEY_FAVORITE_ADD);

    // When/Then: 0번 선호 등록 후 다음선호에서 0으로 이동
    givenCurrentChannel(99);
    thenExpectSetChannel(0);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);
}

TEST_F(TVControllerTest, should_toggle_favorite_at_channel_99) {
    // Given: 경계값 99번 채널
    givenCurrentChannel(99);
    whenPush(remoteKey::KEY_FAVORITE_ADD);

    // When/Then: 99번 선호 등록 후 다음선호에서 99로 이동
    givenCurrentChannel(0);
    thenExpectSetChannel(99);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);
}

TEST_F(TVControllerTest, should_keep_other_favorites_when_removing_one) {
    // Given: 4번·12번 선호 등록, 현재 12번
    givenCurrentChannel(4);
    whenPush(remoteKey::KEY_FAVORITE_ADD);
    givenCurrentChannel(12);
    whenPush(remoteKey::KEY_FAVORITE_ADD);

    // When: 12번 선호 삭제
    whenPush(remoteKey::KEY_FAVORITE_ADD);

    // Then: 6번에서 다음선호 → 4번 (12는 제외)
    givenCurrentChannel(6);
    thenExpectSetChannel(4);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);
}

// ---------------------------------------------------------------------------
// README §3 — 다음 선호 채널 (최소 5)
// ---------------------------------------------------------------------------

TEST_F(TVControllerTest, should_go_to_12_when_on_6_with_favorites_1_4_12_56) {
    // Given: 선호 1,4,12,56 / 현재 6번
    for (int ch : {1, 4, 12, 56}) {
        givenCurrentChannel(ch);
        whenPush(remoteKey::KEY_FAVORITE_ADD);
    }
    givenCurrentChannel(6);
    ASSERT_EQ(6, toChannel(mockTuner.getCurrentCH()));

    // When: 다음선호채널
    thenExpectSetChannel(12);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);
}

TEST_F(TVControllerTest, should_wrap_to_1_when_on_56_with_favorites_1_4_12_56) {
    // Given: 선호 1,4,12,56 / 현재 56번
    for (int ch : {1, 4, 12, 56}) {
        givenCurrentChannel(ch);
        whenPush(remoteKey::KEY_FAVORITE_ADD);
    }
    givenCurrentChannel(56);

    // When: 다음선호채널 (순환)
    thenExpectSetChannel(1);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);
}

TEST_F(TVControllerTest, should_not_call_setch_when_favorite_list_empty) {
    // Given: 선호 목록 없음, 현재 6번
    givenCurrentChannel(6);
    EXPECT_CALL(mockTuner, getCurrentCH()).Times(::testing::AtLeast(1));
    EXPECT_CALL(mockTuner, setCH(::testing::_)).Times(0);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);

    // When/Then: 선호 이동 없이 일반 채널 업은 7번으로 동작
    thenExpectSetChannel(7);
    whenPush(remoteKey::KEY_UP);
}

TEST_F(TVControllerTest, should_go_to_smallest_favorite_above_0_from_channel_0) {
    // Given: 선호 1,4,12 / 현재 0번(경계)
    for (int ch : {1, 4, 12}) {
        givenCurrentChannel(ch);
        whenPush(remoteKey::KEY_FAVORITE_ADD);
    }
    givenCurrentChannel(0);

    // When: 다음선호채널
    thenExpectSetChannel(1);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);
}

TEST_F(TVControllerTest, should_wrap_to_1_from_channel_99_when_only_greater_is_none) {
    // Given: 선호 1,4,12,56 / 현재 99번(경계)
    for (int ch : {1, 4, 12, 56}) {
        givenCurrentChannel(ch);
        whenPush(remoteKey::KEY_FAVORITE_ADD);
    }
    givenCurrentChannel(99);

    // When: 다음선호채널
    thenExpectSetChannel(1);
    whenPush(remoteKey::KEY_FAVORITE_NEXT);
}

// ---------------------------------------------------------------------------
// README §4 — 채널 검색 (최소 5)
// ---------------------------------------------------------------------------

TEST_F(TVControllerTest, should_invoke_seekch_when_search_pressed) {
    // Given: 검색 전 상태
    EXPECT_CALL(mockTuner, seekCH()).Times(::testing::AtLeast(1));

    // When: 채널검색
    whenPush(remoteKey::KEY_SEARCH);
}

TEST_F(TVControllerTest, should_store_search_results_for_navigation) {
    // Given: seekCH가 4→6→14→4 순으로 반환 (종료)
    givenSearchResults({"4", "6", "14"});

    // When/Then: 검색 후 6번에서 업 → 14
    givenCurrentChannel(6);
    thenExpectSetChannel(14);
    whenPush(remoteKey::KEY_UP);
}

TEST_F(TVControllerTest, should_search_from_channel_99) {
    // Given: 현재 99번(경계)
    givenCurrentChannel(99);
    ::testing::InSequence seq;
    EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return("0"));
    EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return("99"));

    // When: 채널검색
    whenPush(remoteKey::KEY_SEARCH);
}

TEST_F(TVControllerTest, should_search_from_channel_0) {
    // Given: 현재 0번(경계)
    givenCurrentChannel(0);
    ::testing::InSequence seq;
    EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return("4"));
    EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return("0"));

    // When: 채널검색
    whenPush(remoteKey::KEY_SEARCH);
}

TEST_F(TVControllerTest, should_deduplicate_duplicate_seek_results) {
    // Given: seekCH가 중복 채널 반환
    ::testing::InSequence seq;
    EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return("6"));
    EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return("6"));
    EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return("14"));
    EXPECT_CALL(mockTuner, seekCH()).WillOnce(::testing::Return("6"));

    // When: 채널검색
    whenPush(remoteKey::KEY_SEARCH);

    // Then: 6번에서 다운 → 14 (중복 6은 1회만 저장)
    givenCurrentChannel(6);
    thenExpectSetChannel(14);
    whenPush(remoteKey::KEY_DOWN);
}

// ---------------------------------------------------------------------------
// README §5 — 업/다운 (검색 결과 없음, 최소 5)
// ---------------------------------------------------------------------------

TEST_F(TVControllerTest, should_go_to_7_and_5_when_up_and_down_from_6_without_search) {
    // Given: 검색 없음, 현재 6번
    givenCurrentChannel(6);
    ::testing::InSequence seq;
    thenExpectSetChannel(7);
    whenPush(remoteKey::KEY_UP);
    givenCurrentChannel(7);
    thenExpectSetChannel(5);
    whenPush(remoteKey::KEY_DOWN);
}

TEST_F(TVControllerTest, should_wrap_to_0_when_up_from_99_without_search) {
    // Given: 경계 99번, 검색 없음
    givenCurrentChannel(99);
    thenExpectSetChannel(0);

    // When: 채널 업
    whenPush(remoteKey::KEY_UP);
}

TEST_F(TVControllerTest, should_wrap_to_99_when_down_from_0_without_search) {
    // Given: 경계 0번, 검색 없음
    givenCurrentChannel(0);
    thenExpectSetChannel(99);

    // When: 채널 다운
    whenPush(remoteKey::KEY_DOWN);
}

TEST_F(TVControllerTest, should_go_to_1_when_up_from_0_without_search) {
    // Given: 0번에서 업 (순환 아님, +1)
    givenCurrentChannel(0);
    thenExpectSetChannel(1);

    // When: 채널 업
    whenPush(remoteKey::KEY_UP);
}

TEST_F(TVControllerTest, should_go_to_98_when_down_from_99_without_search) {
    // Given: 99번에서 다운 (순환 아님, -1)
    givenCurrentChannel(99);
    thenExpectSetChannel(98);

    // When: 채널 다운
    whenPush(remoteKey::KEY_DOWN);
}

// ---------------------------------------------------------------------------
// README §6 — 업/다운 (검색 결과 있음, 최소 5)
// ---------------------------------------------------------------------------

TEST_F(TVControllerTest, should_go_to_14_and_4_when_up_down_from_6_on_search_list) {
    // Given: 검색 결과 4,6,14 / 현재 6번
    givenSearchResults({"4", "6", "14"});
    givenCurrentChannel(6);
    ::testing::InSequence seq;
    thenExpectSetChannel(14);
    whenPush(remoteKey::KEY_UP);
    givenCurrentChannel(14);
    thenExpectSetChannel(4);
    whenPush(remoteKey::KEY_DOWN);
}

TEST_F(TVControllerTest, should_go_to_4_and_14_when_up_down_from_15_off_search_list) {
    // Given: 검색 결과 4,6,14 / 현재 15번(목록 밖)
    givenSearchResults({"4", "6", "14"});
    givenCurrentChannel(15);
    ::testing::InSequence seq;
    thenExpectSetChannel(4);
    whenPush(remoteKey::KEY_UP);
    givenCurrentChannel(4);
    thenExpectSetChannel(14);
    whenPush(remoteKey::KEY_DOWN);
}

TEST_F(TVControllerTest, should_wrap_up_from_14_to_4_on_search_list) {
    // Given: 검색 4,6,14 / 현재 14번(목록 최대)
    givenSearchResults({"4", "6", "14"});
    givenCurrentChannel(14);
    thenExpectSetChannel(4);

    // When: 채널 업 (목록 순환)
    whenPush(remoteKey::KEY_UP);
}

TEST_F(TVControllerTest, should_wrap_down_from_4_to_14_on_search_list) {
    // Given: 검색 4,6,14 / 현재 4번(목록 최소)
    givenSearchResults({"4", "6", "14"});
    givenCurrentChannel(4);
    thenExpectSetChannel(14);

    // When: 채널 다운 (목록 순환)
    whenPush(remoteKey::KEY_DOWN);
}

TEST_F(TVControllerTest, should_navigate_from_0_using_search_list) {
    // Given: 경계 0번, 검색 결과 4,6,14
    givenSearchResults({"4", "6", "14"});
    givenCurrentChannel(0);
    thenExpectSetChannel(4);

    // When: 채널 업 → 검색 목록 내 다음
    whenPush(remoteKey::KEY_UP);
}
