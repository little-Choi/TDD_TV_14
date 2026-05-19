# TDD TV — 테스트 계획서

| 항목 | 내용 |
|---|---|
| 문서 버전 | 1.0 |
| 작성 기준 | [README.md](../README.md), [requirements_analysis.md](requirements_analysis.md) |
| 대상 모듈 | `TVController`, `Tuner`(계약 검증), `remoteKey` |
| 기술 스택 | C++17, Google Test / Google Mock, CMake |
| 커버리지 도구 | gcov, lcov (가능 시) |

---

## 1. 목적 및 범위

### 1.1 목적

셋탑박스 리모컨 입력을 받아 채널을 관리하고 `Tuner` API를 호출하는 **Controller 모듈**의 비즈니스 규칙을 TDD로 검증한다. `Tuner` 본체 구현은 업체 제공으로 가정하며, 단위 테스트에서는 **Mock/Fake**로 대체하고 Controller의 **호출 계약·상태 전이**에 집중한다.

### 1.2 테스트 범위

| 포함 | 제외 |
|---|---|
| `TVController::pushButton()` 및 내부 상태(입력 버퍼, 검색 목록, 선호 채널) | `Tuner` 실제 하드웨어/펌웨어 동작 |
| `Tuner` API 계약 (`setCH`, `getCurrentCH`, `seekCH`) — Mock 기반 | UI/로그 출력(`std::cout`) 검증 |
| 채널 0~99 유효성, 업/다운·검색·선호 채널 규칙 | 리모컨 센서 물리 계층 프로토콜 |

### 1.3 테스트 피라미드

```
        [소수] 통합/시나리오 (선택)
       /        \
  [다수] TVController TEST_F (MockTuner)
       /        \
  [기반] Tuner 계약 TEST_F / TEST_P (MockTuner) — TunerTest.cpp 참고
```

---

## 2. 테스트 환경

### 2.1 빌드·실행

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

또는 개별 실행:

```bash
./build/TunerTest
./build/TVControllerTest
```

### 2.2 Fixture 공통 패턴

Controller 테스트는 `TunerTest.cpp`와 동일하게 **Google Mock** 기반 Fixture를 사용한다.

```cpp
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

    void SetUp() override {
        // 기본: 현재 채널 "0", 검색/선호 목록 초기화
        ON_CALL(mockTuner, getCurrentCH())
            .WillByDefault(::testing::Return("0"));
    }
};
```

- **검증 원칙**: `std::cout` 로그가 아닌 `EXPECT_CALL` / `EXPECT_EQ` / 상태 조회로 판정한다.
- **Tuner 스텁**: `WillOnce`, `WillRepeatedly`, `InSequence`로 `seekCH()` 반환 시퀀스를 제어한다.

---

## 3. 단위 테스트 범위 및 우선순위 (`TEST_F`)

우선순위: **P0(필수·README 직접 명시)** → **P1(요구 분석·경계)** → **P2(회귀·엣지·정책 미정)**

### 3.1 Tuner 계약 테스트 (`TunerTest` — 기존·유지)

| 우선순위 | Fixture / 패턴 | 테스트 ID | 시나리오 | 상태 |
|:---:|---|---|---|:---:|
| P0 | `TEST_F(TunerTest, …)` | TUN-01 | 초기 `getCurrentCH()` → 0~99 | 구현됨 (`initChannel`) |
| P0 | `TEST_P` | TUN-02 | 유효 채널 `setCH` / `getCurrentCH` | 구현됨 (`0,4,5,12,99`) |
| P0 | `TEST_P` | TUN-03 | 무효 채널 `setCH` → `std::invalid_argument` | 구현됨 |
| P1 | `TEST_F` | TUN-04 | `seekCH()` 반복 — 반환값 0~99 | 구현됨 |
| P1 | `TEST_F` | TUN-05 | `setCH("99")` 후 `seekCH()` | 구현됨 |

### 3.2 TVController — 숫자 입력·확정 (README §1)

| 우선순위 | `TEST_F` 이름 (권장) | 테스트 ID | 입력 시퀀스 | 기대 `setCH` 호출 |
|:---:|---|---|---|---|
| P0 | `DigitThenOk_SetsChannel1` | CTL-01 | `KEY_1`, `KEY_OK` | `"1"` 1회 |
| P0 | `TwoDigits_AutoConfirmChannel12` | CTL-02 | `KEY_1`, `KEY_2` | `"12"` 1회 (2자리 즉시 확정) |
| P0 | `FourDigits_Confirm12Then34` | CTL-03 | `KEY_1`,`2`,`3`,`4` | `"12"`, `"34"` 순서 |
| P0 | `ThreeDigitsThenOk_Confirm45Then6` | CTL-04 | `KEY_4`,`5`,`6`, `KEY_OK` | `"45"`, `"6"` |
| P0 | `PendingDigit_InvalidatedByNonDigitNonOk` | CTL-05 | `KEY_4`,`5`,`6`, `KEY_UP` | `"45"`만; `6` 무효; 업 동작만 |
| P0 | `LeadingZero_NormalizesTo7` | CTL-06 | `KEY_0`, `KEY_7` | `"7"` (정규화 형식 프로젝트 합의) |

### 3.3 TVController — 선호 채널 (README §2)

| 우선순위 | `TEST_F` 이름 (권장) | 테스트 ID | 전제 | 기대 동작 |
|:---:|---|---|---|---|
| P0 | `FavoriteAdd_WhenNotFavorite_Adds` | CTL-07 | 현재 CH=10, 목록 비어 있음 | 선호 목록에 10 추가 |
| P0 | `FavoriteAdd_WhenAlreadyFavorite_Removes` | CTL-08 | 10이 이미 선호 | 목록에서 10 제거 (toggle) |
| P1 | `FavoriteAdd_MultipleChannels` | CTL-09 | 여러 채널 순차 추가 | `set` 정렬·중복 없음 |

### 3.4 TVController — 다음 선호 채널 (README §3)

| 우선순위 | `TEST_F` 이름 (권장) | 테스트 ID | 선호 목록 | 현재 CH | 기대 CH |
|:---:|---|---|---|---|---|
| P0 | `FavoriteNext_From6_GoesTo12` | CTL-10 | 1,4,12,56 | 6 | 12 |
| P0 | `FavoriteNext_From56_WrapsTo1` | CTL-11 | 1,4,12,56 | 56 | 1 |
| P1 | `FavoriteNext_EmptyList_NoSetCh` | CTL-12 | (없음) | 임의 | `setCH` 미호출 |
| P2 | `FavoriteNext_SingleFavorite_WrapsSelf` | CTL-13 | {7} | 7 | 7 (정책 확정 후) |

### 3.5 TVController — 채널 검색 (README §4)

| 우선순위 | `TEST_F` 이름 (권장) | 테스트 ID | 시나리오 | 기대 |
|:---:|---|---|---|---|
| P0 | `ChannelSearch_CallsSeekChRepeatedly` | CTL-14 | `KEY_SEARCH` | `seekCH()` N회, 결과 저장 |
| P1 | `ChannelSearch_DeduplicatesChannels` | CTL-15 | `seekCH` 동일 CH 반복 반환 | 목록 중복 1건 |
| P1 | `ChannelSearch_StopsAtTermination` | CTL-16 | 시작 CH 재방문 또는 최대 100회 | 무한 루프 없음 |

### 3.6 TVController — 업/다운 (README §5, §6)

**검색 결과 없음 (일반 ±1, 0↔99 순환)**

| 우선순위 | `TEST_F` 이름 (권장) | 테스트 ID | 현재 CH | 입력 | 기대 CH |
|:---:|---|---|---|---|---|
| P0 | `UpDown_NoSearch_From6` | CTL-17 | 6 | UP / DOWN | 7 / 5 |
| P0 | `UpDown_NoSearch_From99_WrapsTo0` | CTL-18 | 99 | UP | 0 |
| P0 | `UpDown_NoSearch_From0_WrapsTo99` | CTL-19 | 0 | DOWN | 99 |

**검색 결과 있음 (목록 내 다음/이전, 순환)**

| 우선순위 | `TEST_F` 이름 (권장) | 테스트 ID | 검색 목록 | 현재 CH | UP | DOWN |
|:---:|---|---|---|---|---|---|
| P0 | `UpDown_WithSearch_OnListMember` | CTL-20 | 4,6,14 | 6 | 14 | 4 |
| P0 | `UpDown_WithSearch_OffListMember` | CTL-21 | 4,6,14 | 15 | 4 | 14 |

### 3.7 구현 순서 권장 (TDD Red → Green)

1. **P0** CTL-01 ~ CTL-06 (숫자·버퍼) — `remoteKey` 확장과 함께
2. **P0** CTL-17 ~ CTL-19 (업/다운, 검색 없음)
3. **P0** CTL-07 ~ CTL-08, CTL-10 ~ CTL-11 (선호)
4. **P0** CTL-14, CTL-20 ~ CTL-21 (검색·검색 기반 업/다운)
5. **P1** 나머지 및 파라미터화(`TEST_P`)로 경계값 일괄 확장

---

## 4. 경계값 테스트 목록

채널 유효 범위는 **0 ~ 99** (일반 채널). 경계값은 `TEST_P`로 묶어 회귀 비용을 줄인다.

### 4.1 채널 번호 경계

| ID | 입력/상태 | 기대 동작 | 권장 테스트 |
|---|---|---|---|
| BND-01 | CH = **0** | 유효; DOWN → **99** | `TEST_P(ValidChannelBoundary, …)` |
| BND-02 | CH = **99** | 유효; UP → **0** | 동일 |
| BND-03 | `KEY_0` 단독 / `0`+`OK` | 0번 확정 또는 버퍼 정책에 따름 | `TEST_F` |
| BND-04 | `9`,`9` 또는 `99` 입력 | `setCH("99")` | `TEST_F` |
| BND-05 | `1`,`0` (10번) | `setCH("10")` — 앞자리 0과 구분 | `TEST_F` |
| BND-06 | `0`,`7` | **7**로 정규화 (`"07"` → `"7"`) | CTL-06 |
| BND-07 | 100, 9999 | `setCH` **미호출** 또는 예외 차단 | `TEST_P` (TUN-03 연계) |
| BND-08 | -1, -12, -0 | 무효 — Controller 차단 | `TEST_P` |

### 4.2 업/다운 순환 경계

| ID | 현재 CH | 검색 목록 | 동작 | 기대 |
|---|---|---|---|---|
| BND-09 | 0 | 없음 | DOWN | 99 |
| BND-10 | 99 | 없음 | UP | 0 |
| BND-11 | 99 | 없음 | UP 후 DOWN | 0 → 99 |
| BND-12 | 15 | {4,6,14} | UP / DOWN | 4 / 14 (목록 순환) |
| BND-13 | 4 | {4,6,14} | DOWN | 14 (목록 하단 순환) |
| BND-14 | 14 | {4,6,14} | UP | 4 (목록 상단 순환) |

### 4.3 입력 버퍼 경계

| ID | 시퀀스 | 기대 |
|---|---|---|
| BND-15 | 숫자 2개 연속 | 즉시 확정, 버퍼 클리어 |
| BND-16 | 숫자 3개 (`4`,`5`,`6`) | 45 확정, `6` 대기 |
| BND-17 | 대기 중 `6` + 비숫자·비OK | `6` 폐기 |
| BND-18 | `1`,`2`,`3`,`4` | 12 → 34 (2자리 단위) |

### 4.4 검색·선호 경계

| ID | 조건 | 기대 |
|---|---|---|
| BND-19 | `seekCH()` 100회 또는 시작 CH 재방문 | 검색 종료 |
| BND-20 | 선호 목록 비어 있음 + NEXT | `setCH` 없음 (정책 문서화) |
| BND-21 | 선호 1개 + NEXT | 자기 자신 또는 무동작 (팀 합의) |
| BND-22 | 현재 CH = 선호 최댓값 + NEXT | 최솟값 선호로 순환 (56→1) |

---

## 5. 예외·특이 케이스 목록

### 5.1 예외 (Tuner 계약)

| ID | 조건 | Controller 기대 | 검증 방법 |
|---|---|---|---|
| EXC-01 | `setCH("-12")` 등 무효 문자열 | Tuner가 `std::invalid_argument` throw | Mock `Throw`; Controller가 **전파 vs 흡수** 정책 1종 선택 후 테스트 고정 |
| EXC-02 | `getCurrentCH()` 비숫자 반환 | 파싱 실패 처리 | Mock `Return("abc")`; 예외 또는 무시 정책 |
| EXC-03 | `seekCH()` 빈 문자열 | 검색 루프 중단 | `TunerTest` `empty` break 패턴 재사용 |

### 5.2 특이·상태 충돌

| ID | 시나리오 | 기대 |
|---|---|---|
| SPC-01 | 숫자 입력 중 `KEY_SEARCH` | 버퍼 클리어 후 검색 또는 검색 우선 (정책 명시) |
| SPC-02 | 숫자 대기 `6` + `KEY_FAVORITE_ADD` | `6` 무효화 + 현재 시청 CH 기준 선호 toggle |
| SPC-03 | 검색 수행 후 업/다운 | **검색 목록** 모드로 전환 (일반 ±1 아님) |
| SPC-04 | 검색 목록 있는 상태에서 숫자 입력 | 목록 유지 여부·버퍼 독립성 |
| SPC-05 | `KEY_OK`만 연속 입력 | 빈 버퍼 → `setCH` 미호출 |
| SPC-06 | 동일 CH에 `FavoriteAdd` 두 번 | add → remove (toggle) |
| SPC-07 | `FavoriteNext` when 현재 CH가 선호 목록에 포함 | strictly greater 중 최소값 (자신 제외) |
| SPC-08 | 검색 결과 1개 채널만 | UP/DOWN 모두 동일 CH 또는 순환 |
| SPC-09 | `processingCH`에 `OK` 문자열 누적 버그 | `to_string(KEY_OK)` ≠ 채널 숫자 — 구현 시 `OK`는 버퍼에 넣지 않음 |

### 5.3 미정 요구사항 (테스트 전 팀 합의)

| 항목 | 선택지 | 테스트 영향 |
|---|---|---|
| `"07"` Tuner 전달 형식 | `"7"` vs `"07"` | CTL-06 assertion 문자열 |
| 선호 비어 있을 때 NEXT | 무동작 vs 예외 | CTL-12 |
| Controller가 Tuner 예외 흡수 여부 | throw 전파 vs 로그만 | EXC-01 Mock 기대값 |
| 검색 종료 조건 | 100회 cap vs 시작 CH 재방문 | CTL-16 |

---

## 6. 커버리지 목표 및 gcov/lcov 전략

### 6.1 목표

| 지표 | 목표 | 비고 |
|---|---|---|
| **Line coverage** (`src/TVController.cpp` 등 자체 코드) | **≥ 90%** | Mock으로 Tuner 제외 |
| **Branch coverage** | **≥ 85%** | `switch(remoteKey)`, 버퍼/검색 분기 |
| **Function coverage** | **100%** | public API 전부 호출 |
| Tuner 벤더 코드 | 측정 제외 | Mock만 사용 |

미달 시 **릴리스 전 보완**: P1 미구현 `TEST_F` 추가 → Red → Green.

### 6.2 CMake + gcov 설정 (권장)

커버리지 빌드는 별도 디렉터리에서 수행한다.

```bash
cmake -S . -B build-coverage \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build-coverage
cd build-coverage && ctest --output-on-failure
```

`TVController` 구현이 헤더-only(`include/TVController.h`)인 경우, **측정 대상을 명시**한다.

- 옵션 A: 로직을 `src/TVController.cpp`로 분리 후 해당 TU만 `--coverage` 링크
- 옵션 B: 테스트 타깃에 `TVController.cpp` 또는 헤더 구현 TU를 포함하고 gcov가 생성되도록 CMake `target_sources` 정리

### 6.3 lcov 수집·리포트

```bash
# 0으로 초기화 후 테스트 실행으로 .gcda 생성
lcov --directory build-coverage --zerocounters
cd build-coverage && ctest --output-on-failure && cd ..

lcov --directory build-coverage --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/googletest/*' '*/gmock/*' '*/test/*' \
  --output-file coverage.filtered.info

genhtml coverage.filtered.info --output-directory coverage-html
```

Windows에서는 WSL 또는 MSYS2의 `gcov`/`lcov` 사용을 권장한다.

### 6.4 커버리지 개선 절차

1. **기준선 측정**: 필터 적용 후 line/branch % 기록 (CI 아티팩트 또는 `coverage-html/index.html`).
2. **미커버 라인 식별**: `genhtml`에서 빨간 분기 → 대응 `TEST_F` ID 매핑 (본 문서 §3).
3. **우선 보완 순서**:  
   - 미실행 `case` in `pushButton`  
   - `processingCH` 버퍼 클리어/무효화 분기  
   - 검색 종료·중복 제거 분기  
   - 선호 `upper_bound` / wrap 분기  
4. **회귀 방지**: 커버리지 **하한선**을 CI에 설정 (예: line ≥ 88% 이하면 실패).
5. **안티패턴 금지**: 커버리지만 위한 무의미 assert, 로그 출력 검증, Mock 없는 실 Tuner 의존.

### 6.5 CI 연동 (선택)

```yaml
# 예: GitHub Actions 요약
- run: cmake -B build-coverage -DCMAKE_CXX_FLAGS=--coverage ...
- run: cmake --build build-coverage && ctest --test-dir build-coverage
- run: lcov ... && genhtml ...
- uses: codecov/codecov-action@v4  # coverage.filtered.info 업로드
```

---

## 7. 테스트 데이터·Mock 시퀀스 예시

### 7.1 채널 검색 Mock

```cpp
::testing::InSequence seq;
EXPECT_CALL(mockTuner, seekCH()).WillOnce(Return("4"));
EXPECT_CALL(mockTuner, seekCH()).WillOnce(Return("6"));
EXPECT_CALL(mockTuner, seekCH()).WillOnce(Return("14"));
EXPECT_CALL(mockTuner, seekCH()).WillOnce(Return("4")); // 종료 조건
controller.pushButton(remoteKey::KEY_SEARCH);
```

### 7.2 업/다운 (검색 없음)

```cpp
EXPECT_CALL(mockTuner, getCurrentCH()).WillRepeatedly(Return("6"));
EXPECT_CALL(mockTuner, setCH("7")).Times(1);
controller.pushButton(remoteKey::KEY_UP);
```

---

## 8. 완료 기준 (Exit Criteria)

- [ ] README §1~§6에 대응하는 **P0 `TEST_F` 전부 Green**
- [ ] §4 경계값 **BND-01 ~ BND-14** 최소 1개 이상의 테스트로 커버
- [ ] §5 **EXC-01**, **SPC-05**, **SPC-09** 포함 특이 케이스 문서화·구현
- [ ] `TunerTest` + `TVControllerTest` `ctest` 100% pass
- [ ] `TVController` 자체 코드 line coverage **≥ 90%** (lcov 필터 후)
- [ ] 미정 요구사항(§5.3) 팀 합의 및 테스트 assertion 반영

---

## 9. 부록 — README 요구 ↔ 테스트 ID 매트릭스

| README 항목 | 테스트 ID |
|---|---|
| `1` + 확인 → 1번 | CTL-01 |
| `1`,`2` → 12번 | CTL-02 |
| `1`,`2`,`3`,`4` → 12, 34 | CTL-03 |
| `4`,`5`,`6` + 확인/무효 | CTL-04, CTL-05 |
| `0`,`7` → 7번 | CTL-06 |
| 선호 추가/삭제 | CTL-07, CTL-08 |
| 다음 선호 (6→12, 56→1) | CTL-10, CTL-11 |
| 채널 검색 저장 | CTL-14 |
| 업/다운 (검색 없음) | CTL-17 ~ CTL-19 |
| 업/다운 (검색 있음) | CTL-20, CTL-21 |

---

## 10. 변경 이력

| 버전 | 일자 | 변경 내용 |
|---|---|---|
| 1.0 | 2026-05-19 | 초안 작성 (README·요구사항 분석 기반) |
