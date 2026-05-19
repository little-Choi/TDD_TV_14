# 06. Golden Master 회귀 테스트 대화형 프롬프트

## 목적

TDD TV C++17 프로젝트에서 `TVController`의 TextTest 스타일 **출력 기반 Golden Master(Approval)** 회귀 테스트를 설계·구현하고, README §1~§6 전 시나리오를 커버한 뒤 해당 산출물에 대한 보고서와 대화형 프롬프트 transcript를 기록한다.

## Conversation Transcript

### User

```text
@include/TVController.h

[P] 회귀 테스트(Approval/Golden Master) 설계 전문가입니다.
[C] C++17, Google Test, CMake
[T] TexttestFixture 출력 기반 Golden Master를 설계/구현해줘.
    1) expected 출력 파일 생성/보관 전략
    2) Google Test에서 파일 비교 테스트 구현
    3) CMake/ctest에 통합하는 방법
    4) CI에서 자동 실행 구성
[F] 테스트 코드 + CMake 수정안 + 실행 방법
```

### Assistant

```text
TVController.h, CMakeLists.txt, TVControllerTest.cpp, README, 기존 CI 유무를 확인한다.

TextTest 스타일 Golden Master 인프라를 구현한다.
- test/support/GoldenMaster.h … approveOrCompare, UPDATE_GOLDEN, LF 정규화
- test/support/TextTestFixture.h … cout 캡처, Given/When/키 트랜스크립트
- test/fake/FakeTuner.h … 결정적 Tuner (golden 전용)
- test/TVControllerGoldenTest.cpp … README 핵심 시나리오 8건
- test/golden/approved/*.approved.txt, received/.gitignore
- CMakeLists.txt … TVControllerGoldenTest, GOLDEN_FIXTURE_DIR, update-golden, LABELS golden
- .github/workflows/ci.yml … cmake build + ctest

cmake --build build --target update-golden 후 ctest 53/53 통과(단위 45 + golden 8) 확인.
실행 방법·아키텍처·보관 전략을 응답에 정리한다.
```

### User

```text
계속
```

### Assistant

```text
README §1~§6 나머지 시나리오를 Golden Master로 확장한다.

TVControllerGoldenTest.cpp를 리팩터링한다.
- Fixture 헬퍼: pushKey, setChannel, registerFavorites, runSearch, runScenario, finish
- TVControllerTest 32건과 1:1 동일 TEST_F 이름·시나리오
- §1~§6 전부 커버 (32 golden tests)

cmake --build build --target update-golden으로 approved 32개 생성.
ctest: golden 32/32, 전체 77/77 통과.
README.md에 Golden Master 실행 절 추가.
```

### User

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```

### Assistant

```text
기존 Report·Prompting 폴더의 파일명 규칙(01~05)과 문서 형식을 확인한다.

다음 파일을 작성한다.
- Report/06_golden_master_test_report.md
- Prompting/06_golden_master_test_report-Prompt.md

보고서에는 Golden Master 목적·아키텍처·approved/received 전략·
Google Test/CMake/CI 통합·32건 규모·77/77 Green 결과·단위 테스트와 역할 분리·
한계·후속 작업·생성 파일 목록을 포함한다.

프롬프트 파일에는 Golden Master 설계 요청 → 1차 구현(8건) →
「계속」32건 확장 → Report/Prompting export 요청까지의 대화형 transcript를 정리한다.
```

## Expected Output Files

- `test/support/GoldenMaster.h`
- `test/support/TextTestFixture.h`
- `test/fake/FakeTuner.h`
- `test/TVControllerGoldenTest.cpp`
- `test/golden/approved/*.approved.txt` (32)
- `test/golden/received/.gitignore`
- `CMakeLists.txt`
- `.github/workflows/ci.yml`
- `README.md` (Golden Master 절)
- `Report/06_golden_master_test_report.md`
- `Prompting/06_golden_master_test_report-Prompt.md`

## 재사용 프롬프트 (요약)

동일 작업을 다른 브랜치/프로젝트에 적용할 때 아래 P/C/T/F 블록을 그대로 사용할 수 있다.

```text
[P] 회귀 테스트(Approval/Golden Master) 설계 전문가입니다.
[C] C++17, Google Test, CMake
[T] TexttestFixture 출력 기반 Golden Master를 설계/구현해줘.
    1) expected 출력 파일 생성/보관 전략 (approved/received, UPDATE_GOLDEN)
    2) Google Test에서 파일 비교 테스트 구현
    3) CMake/ctest에 통합 (update-golden 타겟, LABELS golden)
    4) CI에서 자동 실행 구성
    5) 기존 단위 테스트 시나리오와 1:1 golden 확장
[F] 테스트 코드 + CMake 수정 + approved baseline + 실행 방법 + Report/Prompting export
```
