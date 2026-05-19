# 03. 테스트 계획 대화형 프롬프트

## 목적

TDD TV C++17 프로젝트의 `README.md`와 요구사항 분석 문서를 기준으로 시니어 QA 리드 관점의 테스트 계획서를 작성하고, 해당 산출물에 대한 보고서와 대화형 프롬프트 transcript를 기록한다.

## Conversation Transcript

### User

```text
@README.md @docs/requirements_analysis.md

[P] 시니어 QA 리드입니다.
[C] C++17, Google Test, CMake, (가능하면 gcov/lcov)
[T] 테스트 계획서를 작성해줘.
    - READ.md 의 동작 설명 기반으로 작성
    - TEST_F 기반 단위 테스트 범위/우선순위
    - 경계값 케이스 목록 (채널 0,99)
    - 예외/특이 케이스 목록
    - 커버리지 목표 (예: 90%+)와 gcov/lcov 측정·개선 전략
[F] Markdown 문서. docs 폴더에 test_plan.md로 저장
```

### Assistant

```text
README.md, docs/requirements_analysis.md, 기존 테스트·헤더·CMake 구성을 확인한다.

확인 대상:
- README.md
- docs/requirements_analysis.md
- test/TunerTest.cpp
- test/TVControllerTest.cpp
- include/TVController.h, Tuner.h, remoteKey.h
- CMakeLists.txt

docs/test_plan.md를 작성한다. 포함 내용:
- 목적·범위·테스트 피라미드
- TEST_F 우선순위(P0~P2): Tuner(TUN), TVController(CTL)
- 경계값 목록(BND): 채널 0/99, 순환, 버퍼, 검색·선호
- 예외·특이(EXC/SPC) 및 미정 정책
- 커버리지 목표(90%+), gcov/lcov CMake·lcov·genhtml·개선 절차
- Mock Fixture 예시, Exit Criteria, README↔테스트 ID 매트릭스
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
- Prompting/01_cursorrules_report-Prompt.md
- Prompting/02_requirements_analysis_report-Prompt.md

세 번째 산출물로 다음 파일을 작성한다.
- Report/03_test_plan_report.md
- Prompting/03_test_plan_report-Prompt.md

보고서에는 테스트 계획의 목적, 범위, 주요 산출(우선순위·경계값·예외·커버리지), QA 기대 효과, 생성 파일 목록을 포함한다.

프롬프트 파일에는 테스트 계획 작성 요청부터 Report/Prompting export 요청까지의 대화형 transcript를 정리한다.
```

## Expected Output Files

- `docs/test_plan.md`
- `Report/03_test_plan_report.md`
- `Prompting/03_test_plan_report-Prompt.md`
