# 04. TVController 단위 테스트 보고서

## 개요

- 대상 프로젝트: TDD TV C++17 프로젝트
- 작성 파일: `test/TVControllerTest.cpp`, `include/remoteKey.h` (컴파일용 키 확장)
- 목적: `README.md` §1~§6 동작을 Google Test/Mock 기반 `TEST_F`로 명세화하고 TDD Red 상태 확보
- 작성일: 2026-05-19
- 작성 역할: 시니어 C++ QA (테스트 설계)

## 작업 범위

`README.md`에 정의된 6가지 Controller 동작(숫자 입력·선호 추가·다음 선호·채널 검색·업/다운 검색 유/무)을 `TVController::pushButton()` 계약 검증 대상으로 삼았다. `Tuner`는 `MockTunerForController`로 대체하며, `std::cout` 로그는 검증하지 않는다.

기존 `docs/test_plan.md`, `docs/requirements_analysis.md`의 CTL/BND 시나리오와 `.cursorrules`의 Given-When-Then·`should_*_when_*` 명명 규칙을 따랐다.

## 주요 산출 내용

### 테스트 규모

| README 동작 | 테스트 수 | 대표 시나리오 |
|---|---:|---|
| §1 숫자 버튼 채널 변경 | 7 | `1+OK→1`, `12` 자동확정, `1234→12/34`, `456+OK→45/6`, `07→7`, `99` 경계, `456+UP` 대기 숫자 무효화 |
| §2 선호 채널 추가 | 5 | 추가/삭제 토글, 다중 선호 유지 |
| §3 다음 선호 채널 | 5 | 6→12, 56→1 순환, 빈 목록·0/99 경계 |
| §4 채널 검색 | 5 | `seekCH` 호출, 결과 저장·업/다운 연동, 중복 제거 |
| §5 업/다운 (검색 없음) | 5 | 6→7/5, 99↑→0, 0↓→99 |
| §6 업/다운 (검색 있음) | 5 | 목록 `{4,6,14}` on/off-list·순환 |
| **합계** | **32** | 동작별 최소 5개 요구 충족 |

### Fixture·검증 패턴

- `TVControllerTest` + `MockTunerForController` (`TunerTest.cpp`와 동일 Mock 3메서드)
- Given-When-Then 주석 구조
- 채널 검증: `thenExpectSetChannel(int)` → `setCH` 호출 시 **`ASSERT_EQ(expected, std::stoi(ch))`**
- 검색 목록 시드: `givenSearchResults()` — `seekCH()` 시퀀스 Mock 후 `KEY_SEARCH`
- 테스트 이름: `should_[result]_when_[condition]`

### 경계값 반영

- 채널 **0**, **99**: 숫자 입력·선호 토글·다음 선호·검색 시작·업/다운(검색 유/무) 테스트에 포함
- README §1 `4,5,6` + 비숫자 버튼 시 `6` 무효화: `should_confirm_45_only_when_key_up_after_digits_4_5_6`

### `remoteKey` 확장

테스트에서 참조하는 모든 리모컨 키를 `enum class remoteKey`에 추가했다 (`KEY_0`~`KEY_9`, `KEY_OK`, `KEY_UP`, `KEY_DOWN`, `KEY_SEARCH`, `KEY_FAVORITE_ADD`, `KEY_FAVORITE_NEXT`). `TVController` 구현은 미완성 상태를 유지한다.

## TDD 상태 (Red)

| 대상 | 결과 |
|---|---|
| `TunerTest` | 12/12 통과 (기존) |
| `TVControllerTest` | **32/32 실패** (의도된 Red) |

현재 `TVController`는 `KEY_1`·`KEY_OK`만 처리하며 `tuner->setCH()` 호출이 주석 처리되어 있어, Mock `EXPECT_CALL` 기대와 불일치한다.

빌드·실행 예 (MinGW):

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build-mingw
ctest --test-dir build-mingw -R TVController --output-on-failure
```

## QA 관점 기대 효과

- README §1~§6과 `TEST_F`를 1:1로 추적할 수 있어 Green 구현 시 회귀 기준이 된다.
- Mock 계약 검증으로 Tuner 벤더 코드 없이 Controller 로직만 TDD할 수 있다.
- 경계값(0/99)·버퍼 무효화 등 실무 결함 다발 구간을 사전에 고정했다.
- `docs/test_plan.md`의 CTL-01~21 시나리오 대부분이 실행 가능한 테스트 코드로 구체화되었다.

## 후속 작업 (Green 단계)

1. `TVController::pushButton()` — 숫자 버퍼·2자리 확정·`OK`·앞자리 0 정규화
2. 업/다운 — 검색 목록 유/무 분기 및 0↔99 순환
3. 선호 채널 `set` + `upper_bound` 기반 다음 선호
4. 채널 검색 — `seekCH()` 반복·종료·중복 제거
5. P0 `TVControllerTest` 전부 Green 후 `ctest` 100% pass

## 생성 파일

- `test/TVControllerTest.cpp`
- `include/remoteKey.h` (키 enum 확장)
- `Report/04_tv_controller_test_report.md`
- `Prompting/04_tv_controller_test_report-Prompt.md`
