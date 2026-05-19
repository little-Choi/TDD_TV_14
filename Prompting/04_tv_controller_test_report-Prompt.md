# 04. TVController 단위 테스트 대화형 프롬프트

## 목적

TDD TV C++17 프로젝트의 `README.md` 동작을 기준으로 `TVController` Google Test/Mock 단위 테스트를 작성하고, TDD Red 상태를 확보한 뒤 해당 산출물에 대한 보고서와 대화형 프롬프트 transcript를 기록한다.

## Conversation Transcript

### User

```text
@test/TVControllerTest.cpp @include/TVController.h @include/Tuner.h @include/remoteKey.h @docs/requirements_analysis.md

[P] 테스트 설계에 강한 시니어 C++ QA입니다.
[C] C++17, Google Test
[T] README.md의 동작별 최소 5개 TEST를 작성해줘.
    - EXPECT_EQ/ASSERT_EQ로 채널 검증
    - 경계값 포함 (채널 0/99)
    - Given-When-Then 주석 구조 사용
[F] 완성된 테스트 코드. ctest는 Green이 아니어 됨.
```

### Assistant

```text
README.md, requirements_analysis.md, 기존 TunerTest/TVControllerTest 패턴,
TVController.h, remoteKey.h, test_plan.md를 확인한다.

test/TVControllerTest.cpp에 README §1~§6 동작별 최소 5개 TEST_F(총 32개)를 작성한다.
- MockTunerForController + TVControllerTest Fixture
- Given-When-Then 주석, should_*_when_* 명명
- thenExpectSetChannel(int)에서 setCH 호출 시 ASSERT_EQ로 채널 정수 검증
- 경계값 0/99를 숫자·선호·검색·업/다운 테스트에 반영
- §1 '456+UP' 대기 숫자 무효화 시나리오 추가

컴파일을 위해 include/remoteKey.h에 KEY_0~9, UP/DOWN, SEARCH, 선호 키 enum 확장.
TVController 구현은 변경하지 않음(setCH 주석 상태 유지).

MinGW로 빌드·ctest 실행:
- TVControllerTest 32/32 실패(Red) 확인
- TunerTest 12/12 통과
- vacuous pass 방지를 위해 선호 빈 목록·삭제 테스트에 getCurrentCH/setCH 양성 기대 보강
```

### User

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```

### Assistant

```text
기존 Report·Prompting 폴더의 파일명 규칙과 문서 형식을 확인한다.

기존 파일:
- Report/01_cursorrules_report.md
- Report/02_requirements_analysis_report.md
- Report/03_test_plan_report.md
- Prompting/01_cursorrules_report-Prompt.md
- Prompting/02_requirements_analysis_report-Prompt.md
- Prompting/03_test_plan_report-Prompt.md

네 번째 산출물로 다음 파일을 작성한다.
- Report/04_tv_controller_test_report.md
- Prompting/04_tv_controller_test_report-Prompt.md

보고서에는 테스트 작성 목적·범위·동작별 32개 TEST_F 요약·Fixture/검증 패턴·
경계값·remoteKey 확장·TDD Red 상태·QA 기대 효과·Green 후속 작업·생성 파일 목록을 포함한다.

프롬프트 파일에는 TVController 테스트 작성 요청부터 Report/Prompting export 요청까지의
대화형 transcript를 정리한다.
```

## Expected Output Files

- `test/TVControllerTest.cpp`
- `include/remoteKey.h`
- `Report/04_tv_controller_test_report.md`
- `Prompting/04_tv_controller_test_report-Prompt.md`
