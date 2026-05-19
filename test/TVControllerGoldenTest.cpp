#include "TextTestFixture.h"
#include "FakeTuner.h"
#include "TVController.h"
#include "remoteKey.h"
#include <initializer_list>
#include <string>
#include <vector>

class TVControllerGoldenTest : public TextTestFixture {
protected:
    FakeTuner tuner;
    TVController controller{&tuner};

    void pushKey(remoteKey key) {
        logKey(to_string(key));
        controller.pushButton(key);
    }

    void setChannel(int channel) { tuner.setInitialChannel(channel); }

    void logFinal() {
        logLine("# Final: channel=" + std::to_string(tuner.currentChannel()));
    }

    void registerFavorites(std::initializer_list<int> channels) {
        for (int ch : channels) {
            setChannel(ch);
            pushKey(remoteKey::KEY_FAVORITE_ADD);
        }
    }

    void runSearch(const std::vector<std::string>& seekSequence) {
        tuner.setSeekSequence(seekSequence);
        pushKey(remoteKey::KEY_SEARCH);
    }

    void runScenario(const std::vector<remoteKey>& keys) {
        for (remoteKey key : keys) {
            pushKey(key);
        }
        logFinal();
    }

    void finish(const char* testName) { approveGolden(testName); }
};

// README §1 — 숫자 버튼
TEST_F(TVControllerGoldenTest, should_set_channel_1_when_digit_1_and_ok) {
    given("initial channel 0");
    setChannel(0);
    when("press 1, OK");
    runScenario({remoteKey::KEY_1, remoteKey::KEY_OK});
    finish("should_set_channel_1_when_digit_1_and_ok");
}

TEST_F(TVControllerGoldenTest, should_set_channel_12_when_digits_1_and_2) {
    given("initial channel 0");
    setChannel(0);
    when("press 1, 2");
    runScenario({remoteKey::KEY_1, remoteKey::KEY_2});
    finish("should_set_channel_12_when_digits_1_and_2");
}

TEST_F(TVControllerGoldenTest, should_set_channels_12_then_34_when_four_digits) {
    given("initial channel 0");
    setChannel(0);
    when("press 1,2,3,4");
    runScenario({remoteKey::KEY_1, remoteKey::KEY_2, remoteKey::KEY_3, remoteKey::KEY_4});
    finish("should_set_channels_12_then_34_when_four_digits");
}

TEST_F(TVControllerGoldenTest, should_set_channels_45_then_6_when_digits_4_5_6_and_ok) {
    given("initial channel 0");
    setChannel(0);
    when("press 4, 5, 6, OK");
    runScenario({remoteKey::KEY_4, remoteKey::KEY_5, remoteKey::KEY_6, remoteKey::KEY_OK});
    finish("should_set_channels_45_then_6_when_digits_4_5_6_and_ok");
}

TEST_F(TVControllerGoldenTest, should_set_channel_7_when_digits_0_and_7) {
    given("initial channel 0");
    setChannel(0);
    when("press 0, 7");
    runScenario({remoteKey::KEY_0, remoteKey::KEY_7});
    finish("should_set_channel_7_when_digits_0_and_7");
}

TEST_F(TVControllerGoldenTest, should_set_channel_99_when_digits_9_and_9) {
    given("initial channel 0");
    setChannel(0);
    when("press 9, 9");
    runScenario({remoteKey::KEY_9, remoteKey::KEY_9});
    finish("should_set_channel_99_when_digits_9_and_9");
}

TEST_F(TVControllerGoldenTest, should_confirm_45_only_when_key_up_after_digits_4_5_6) {
    given("initial channel 0");
    setChannel(0);
    when("press 4,5,6 then UP from channel 6");
    runScenario({remoteKey::KEY_4, remoteKey::KEY_5, remoteKey::KEY_6});
    setChannel(6);
    pushKey(remoteKey::KEY_UP);
    logFinal();
    finish("should_confirm_45_only_when_key_up_after_digits_4_5_6");
}

// README §2 — 선호 채널 추가
TEST_F(TVControllerGoldenTest, should_add_favorite_when_current_channel_not_in_list) {
    given("channel 10, empty favorites");
    setChannel(10);
    when("FAVORITE_ADD then FAVORITE_NEXT from channel 5");
    pushKey(remoteKey::KEY_FAVORITE_ADD);
    setChannel(5);
    pushKey(remoteKey::KEY_FAVORITE_NEXT);
    logFinal();
    finish("should_add_favorite_when_current_channel_not_in_list");
}

TEST_F(TVControllerGoldenTest, should_remove_favorite_when_current_channel_already_in_list) {
    given("favorite 10 registered then removed");
    setChannel(10);
    pushKey(remoteKey::KEY_FAVORITE_ADD);
    setChannel(5);
    pushKey(remoteKey::KEY_FAVORITE_NEXT);
    setChannel(10);
    pushKey(remoteKey::KEY_FAVORITE_ADD);
    when("FAVORITE_NEXT from 5 after removal");
    setChannel(5);
    pushKey(remoteKey::KEY_FAVORITE_NEXT);
    logFinal();
    finish("should_remove_favorite_when_current_channel_already_in_list");
}

TEST_F(TVControllerGoldenTest, should_toggle_favorite_at_channel_0) {
    given("favorite at channel 0");
    setChannel(0);
    pushKey(remoteKey::KEY_FAVORITE_ADD);
    when("FAVORITE_NEXT from channel 99");
    setChannel(99);
    pushKey(remoteKey::KEY_FAVORITE_NEXT);
    logFinal();
    finish("should_toggle_favorite_at_channel_0");
}

TEST_F(TVControllerGoldenTest, should_toggle_favorite_at_channel_99) {
    given("favorite at channel 99");
    setChannel(99);
    pushKey(remoteKey::KEY_FAVORITE_ADD);
    when("FAVORITE_NEXT from channel 0");
    setChannel(0);
    pushKey(remoteKey::KEY_FAVORITE_NEXT);
    logFinal();
    finish("should_toggle_favorite_at_channel_99");
}

TEST_F(TVControllerGoldenTest, should_keep_other_favorites_when_removing_one) {
    given("favorites 4,12; remove 12");
    registerFavorites({4, 12});
    setChannel(12);
    pushKey(remoteKey::KEY_FAVORITE_ADD);
    when("FAVORITE_NEXT from channel 6");
    setChannel(6);
    pushKey(remoteKey::KEY_FAVORITE_NEXT);
    logFinal();
    finish("should_keep_other_favorites_when_removing_one");
}

// README §3 — 다음 선호 채널
TEST_F(TVControllerGoldenTest, should_go_to_12_when_on_6_with_favorites_1_4_12_56) {
    given("favorites 1,4,12,56");
    registerFavorites({1, 4, 12, 56});
    setChannel(6);
    when("FAVORITE_NEXT");
    runScenario({remoteKey::KEY_FAVORITE_NEXT});
    finish("should_go_to_12_when_on_6_with_favorites_1_4_12_56");
}

TEST_F(TVControllerGoldenTest, should_wrap_to_1_when_on_56_with_favorites_1_4_12_56) {
    given("favorites 1,4,12,56; current 56");
    registerFavorites({1, 4, 12, 56});
    setChannel(56);
    when("FAVORITE_NEXT wrap");
    runScenario({remoteKey::KEY_FAVORITE_NEXT});
    finish("should_wrap_to_1_when_on_56_with_favorites_1_4_12_56");
}

TEST_F(TVControllerGoldenTest, should_not_call_setch_when_favorite_list_empty) {
    given("no favorites, channel 6");
    setChannel(6);
    when("FAVORITE_NEXT then UP");
    pushKey(remoteKey::KEY_FAVORITE_NEXT);
    pushKey(remoteKey::KEY_UP);
    logFinal();
    finish("should_not_call_setch_when_favorite_list_empty");
}

TEST_F(TVControllerGoldenTest, should_go_to_smallest_favorite_above_0_from_channel_0) {
    given("favorites 1,4,12; current 0");
    registerFavorites({1, 4, 12});
    setChannel(0);
    when("FAVORITE_NEXT");
    runScenario({remoteKey::KEY_FAVORITE_NEXT});
    finish("should_go_to_smallest_favorite_above_0_from_channel_0");
}

TEST_F(TVControllerGoldenTest, should_wrap_to_1_from_channel_99_when_only_greater_is_none) {
    given("favorites 1,4,12,56; current 99");
    registerFavorites({1, 4, 12, 56});
    setChannel(99);
    when("FAVORITE_NEXT");
    runScenario({remoteKey::KEY_FAVORITE_NEXT});
    finish("should_wrap_to_1_from_channel_99_when_only_greater_is_none");
}

// README §4 — 채널 검색
TEST_F(TVControllerGoldenTest, should_invoke_seekch_when_search_pressed) {
    given("seek returns 5 then end");
    setChannel(0);
    tuner.setSeekSequence({"5", "5"});
    when("SEARCH");
    pushKey(remoteKey::KEY_SEARCH);
    logFinal();
    finish("should_invoke_seekch_when_search_pressed");
}

TEST_F(TVControllerGoldenTest, should_store_search_results_for_navigation) {
    given("search list 4,6,14");
    setChannel(0);
    runSearch({"4", "6", "14", "4"});
    when("UP from channel 6");
    setChannel(6);
    pushKey(remoteKey::KEY_UP);
    logFinal();
    finish("should_store_search_results_for_navigation");
}

TEST_F(TVControllerGoldenTest, should_search_from_channel_99) {
    given("channel 99");
    setChannel(99);
    when("SEARCH from 99");
    runSearch({"0", "99"});
    logFinal();
    finish("should_search_from_channel_99");
}

TEST_F(TVControllerGoldenTest, should_search_from_channel_0) {
    given("channel 0");
    setChannel(0);
    when("SEARCH from 0");
    runSearch({"4", "0"});
    logFinal();
    finish("should_search_from_channel_0");
}

TEST_F(TVControllerGoldenTest, should_deduplicate_duplicate_seek_results) {
    given("duplicate seek 6,6,14,6");
    setChannel(0);
    runSearch({"6", "6", "14", "6"});
    when("DOWN from channel 6");
    setChannel(6);
    pushKey(remoteKey::KEY_DOWN);
    logFinal();
    finish("should_deduplicate_duplicate_seek_results");
}

// README §5 — 업/다운 (검색 없음)
TEST_F(TVControllerGoldenTest, should_go_to_7_and_5_when_up_and_down_from_6_without_search) {
    given("channel 6");
    setChannel(6);
    when("UP then DOWN");
    runScenario({remoteKey::KEY_UP, remoteKey::KEY_DOWN});
    finish("should_go_to_7_and_5_when_up_and_down_from_6_without_search");
}

TEST_F(TVControllerGoldenTest, should_wrap_to_0_when_up_from_99_without_search) {
    given("channel 99");
    setChannel(99);
    when("UP");
    runScenario({remoteKey::KEY_UP});
    finish("should_wrap_to_0_when_up_from_99_without_search");
}

TEST_F(TVControllerGoldenTest, should_wrap_to_99_when_down_from_0_without_search) {
    given("channel 0");
    setChannel(0);
    when("DOWN wrap to 99");
    runScenario({remoteKey::KEY_DOWN});
    finish("should_wrap_to_99_when_down_from_0_without_search");
}

TEST_F(TVControllerGoldenTest, should_go_to_1_when_up_from_0_without_search) {
    given("channel 0");
    setChannel(0);
    when("UP to 1");
    runScenario({remoteKey::KEY_UP});
    finish("should_go_to_1_when_up_from_0_without_search");
}

TEST_F(TVControllerGoldenTest, should_go_to_98_when_down_from_99_without_search) {
    given("channel 99");
    setChannel(99);
    when("DOWN to 98");
    runScenario({remoteKey::KEY_DOWN});
    finish("should_go_to_98_when_down_from_99_without_search");
}

// README §6 — 업/다운 (검색 결과 있음)
TEST_F(TVControllerGoldenTest, should_go_to_14_and_4_when_up_down_from_6_on_search_list) {
    given("search 4,6,14");
    setChannel(6);
    runSearch({"4", "6", "14", "4"});
    setChannel(6);
    when("UP then DOWN from 6");
    runScenario({remoteKey::KEY_UP, remoteKey::KEY_DOWN});
    finish("should_go_to_14_and_4_when_up_down_from_6_on_search_list");
}

TEST_F(TVControllerGoldenTest, should_go_to_4_and_14_when_up_down_from_15_off_search_list) {
    given("search 4,6,14; current 15");
    setChannel(15);
    runSearch({"4", "6", "14", "4"});
    when("UP then DOWN from 15");
    runScenario({remoteKey::KEY_UP, remoteKey::KEY_DOWN});
    finish("should_go_to_4_and_14_when_up_down_from_15_off_search_list");
}

TEST_F(TVControllerGoldenTest, should_wrap_up_from_14_to_4_on_search_list) {
    given("search 4,6,14; current 14");
    setChannel(14);
    runSearch({"4", "6", "14", "4"});
    when("UP wrap to 4");
    runScenario({remoteKey::KEY_UP});
    finish("should_wrap_up_from_14_to_4_on_search_list");
}

TEST_F(TVControllerGoldenTest, should_wrap_down_from_4_to_14_on_search_list) {
    given("search 4,6,14; current 4");
    setChannel(4);
    runSearch({"4", "6", "14", "4"});
    when("DOWN wrap to 14");
    runScenario({remoteKey::KEY_DOWN});
    finish("should_wrap_down_from_4_to_14_on_search_list");
}

TEST_F(TVControllerGoldenTest, should_navigate_from_0_using_search_list) {
    given("search 4,6,14; current 0");
    setChannel(0);
    runSearch({"4", "6", "14", "4"});
    when("UP from 0");
    runScenario({remoteKey::KEY_UP});
    finish("should_navigate_from_0_using_search_list");
}
