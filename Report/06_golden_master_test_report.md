# 06. Golden Master 회귀 테스트 보고서

## 개요

- 대상 프로젝트: TDD TV C++17 프로젝트
- 작성 파일: `test/TVControllerGoldenTest.cpp`, `test/support/GoldenMaster.h`, `test/support/TextTestFixture.h`, `test/fake/FakeTuner.h`, `test/golden/approved/*.approved.txt`, `CMakeLists.txt`, `.github/workflows/ci.yml`, `README.md` (Golden Master 절)
- 목적: TextTest 스타일 **출력 기반 Golden Master(Approval)** 회귀 테스트를 설계·구현하고 README §1~§6 시나리오를 `TVControllerTest`와 1:1로 커버
- 작성일: 2026-05-19
- 작성 역할: 회귀 테스트(Approval/Golden Master) 설계 전문가

## 작업 범위

`05_tv_controller_implementation_report.md`에서 Green이 된 `TVController`(header-only)의 **관찰 가능한 동작**을 회귀 기준으로 고정한다. 단위 테스트(`TVControllerTest`)는 Mock으로 `setCH` 호출 계약을 검증하고, Golden Master는 `FakeTuner` + **시나리오 트랜스크립트 + `std::cout`** 전체 출력을 승인 파일과 비교한다.

1차 산출(8건) 후 사용자 요청(「계속」)에 따라 **32건**으로 확장하여 `TVControllerTest` 32개 `TEST_F`와 동일한 시나리오명·Given-When-Then 구조를 유지했다.

## 아키텍처

```text
TVControllerGoldenTest (TEST_F)
    └── TextTestFixture          … cout 캡처, Given/When/키 로그
            └── golden::approveOrCompare (GoldenMaster.h)
                    ├── approved: test/golden/approved/<name>.approved.txt  (VCS)
                    └── received: test/golden/received/<name>.received.txt    (실패 시, gitignore)

FakeTuner (결정적 seek 시퀀스) → TVController::pushButton()
```

### 단위 vs Golden 역할 분리

| 항목 | TVControllerTest | TVControllerGoldenTest |
|---|---|---|
| Tuner | `MockTunerForController` (gmock) | `FakeTuner` |
| 검증 | `EXPECT_CALL(setCH)` / `ASSERT_EQ` | 파일 전체 diff |
| stdout | 미검증 | `--- stdout ---` 블록 포함 |
| 목적 | 계약·분기 TDD | 출력 회귀(Approval) |

## 주요 산출 내용

### 1. Expected 출력 보관 전략

| 경로 | 역할 | Git |
|---|---|---|
| `test/golden/approved/<테스트명>.approved.txt` | 승인된 기준 출력 | **커밋 대상** |
| `test/golden/received/<테스트명>.received.txt` | 실패 시 실제 출력 | `.gitignore` (`received/*`) |

- **파일명**: Google Test `TEST_F` 이름과 동일 + `.approved.txt` 접미사
- **트랜스크립트 형식** (TextTest 유사):
  - `# Given:` / `# When:` / `> KEY` / `# Final: channel=N`
  - `--- stdout ---` 이하 `TVController`의 `현재 설정하는 채널 : N` 로그
- **갱신**: `UPDATE_GOLDEN=1` 또는 CMake 타겟 `update-golden` → `approved` 덮어쓰기 후 diff 검토·커밋
- **비교**: LF 정규화(`\r` 제거) 후 문자열 일치 비교

### 2. Google Test 파일 비교 구현

- `GoldenMaster.h`: `approveOrCompare()`, `shouldUpdateGolden()`, `GOLDEN_FIXTURE_DIR` 매크로
- `TextTestFixture.h`: `SetUp`/`TearDown`에서 `std::cout` 리다이렉트, `approveGolden()`
- `TVControllerGoldenTest.cpp` Fixture 헬퍼:
  - `pushKey`, `setChannel`, `registerFavorites`, `runSearch`, `runScenario`, `finish`

### 3. CMake / ctest 통합

- 실행 파일: `TVControllerGoldenTest` (`GTest::gtest_main`, gmock 없음)
- `gtest_discover_tests(TVControllerGoldenTest PROPERTIES LABELS "golden")`
- 커스텀 타겟: `update-golden` (`UPDATE_GOLDEN=1`로 전 테스트 재실행)
- include: `test/support`, `test/fake`

### 4. CI 자동 실행

- `.github/workflows/ci.yml`: Ubuntu, `cmake` configure/build, `ctest --test-dir build --output-on-failure`
- Golden 32건은 전체 ctest에 포함 (별도 job 없이 동일 파이프라인)

## 테스트 규모

| README 동작 | Golden 테스트 수 | 비고 |
|---|---:|---|
| §1 숫자 버튼 | 7 | `TVControllerTest`와 동일 시나리오명 |
| §2 선호 추가 | 5 | 토글·삭제 후 NEXT 무출력 등 트랜스크립트로 기록 |
| §3 다음 선호 | 5 | 6→12, 56→1 wrap, 0/99 경계 |
| §4 채널 검색 | 5 | `FakeTuner::setSeekSequence`로 seek 패턴 고정 |
| §5 업/다운 (검색 없음) | 5 | 0↔99 wrap 포함 |
| §6 업/다운 (검색 있음) | 5 | `{4,6,14}` 목록·off-list·순환 |
| **합계** | **32** | `approved` 파일 32개 |

## 실행 결과 (Green)

| 대상 | 결과 |
|---|---|
| `TunerTest` | 13/13 통과 |
| `TVControllerTest` | 32/32 통과 |
| `TVControllerGoldenTest` | 32/32 통과 |
| **ctest 합계** | **77/77 통과** |

빌드·실행 예 (Windows / MinGW):

```powershell
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -L golden --output-on-failure

# 의도된 출력 변경 후
cmake --build build --target update-golden
```

## QA 관점 기대 효과

- Mock 단위 테스트와 **독립된 2차 안전망**: 리팩터링 시 `cout` 문구·최종 채널·키 시퀀스 회귀를 한 번에 감지
- TextTest 스타일 트랜스크립트로 **비개발자·QA 리뷰**에 유리한 diff
- `update-golden` 워크플로로 Approval 갱신 경로가 명확
- CI에서 `approved` 파일 누락·불일치 시 즉시 실패

## 한계 및 주의사항

- Golden은 **한국어 stdout 문자열**(`현재 설정하는 채널 : `)에 의존 → 로그 문구 변경 시 전체 baseline 갱신 필요
- `FakeTuner` seek 시퀀스는 Mock `givenSearchResults()` 종료 패턴(`…, front`)을 수동으로 맞춰야 함
- 단위 테스트와 Golden이 **동일 시나리오를 이중 검증**하므로, Green 구현 변경 시 두 suite를 함께 확인할 것

## 후속 작업 (선택)

1. `docs/test_plan.md`에 Golden Master 절 및 CTL 시나리오 ↔ `approved` 파일 매핑 표 추가
2. 실패 시 unified diff 출력 (`GoldenMaster.h` 확장)
3. Windows runner를 CI matrix에 추가

## 생성·수정 파일

- `test/TVControllerGoldenTest.cpp`
- `test/support/GoldenMaster.h`
- `test/support/TextTestFixture.h`
- `test/fake/FakeTuner.h`
- `test/golden/approved/*.approved.txt` (32)
- `test/golden/received/.gitignore`
- `CMakeLists.txt`
- `.github/workflows/ci.yml`
- `README.md` (Golden Master 실행 절)
- `Report/06_golden_master_test_report.md`
- `Prompting/06_golden_master_test_report-Prompt.md`
