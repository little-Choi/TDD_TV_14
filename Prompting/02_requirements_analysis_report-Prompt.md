# 02. 요구사항 분석 대화형 프롬프트

## 목적

TDD TV C++17 프로젝트의 `README.md` 요구사항을 시니어 C++ QA 엔지니어 관점에서 구현 및 테스트 기준으로 재정리하고, 해당 산출물에 대한 보고서와 대화형 프롬프트 transcript를 기록한다.

## Conversation Transcript

### User

```text
@README.md

[P] 시니어 C++ QA 엔지니어 관점에서
[C] TDD TV C++17 프로젝트 (CMake, Google Test)
[T] 요구사항을 C++ 구현 관점에서 재정리해줘.
    1) 모듈별 비즈니스 규칙 표
       - 리모컨 / 튜너
    2) 리모컨 버튼 입력 시 주의점
    3) 예외/경계값 조건 (채널 0~99, 선호 채널 등)
    4) Google Test 기준 테스트 시나리오 목록 (번호)
[F] Markdown (표 + 번호 목록). docs 폴더에 requirements_analysis.md로 저장
```

### Assistant

```text
README.md와 관련 헤더/테스트 파일을 읽어 프로젝트 요구사항과 현재 인터페이스를 확인한다.

확인 대상:
- README.md
- include/TVController.h
- include/Tuner.h
- include/remoteKey.h
- test/TunerTest.cpp
- test/TVControllerTest.cpp

docs 폴더가 없으면 생성하고, 다음 내용을 포함한
docs/requirements_analysis.md 파일을 작성한다.

작성 내용:
- 리모컨/TVController 비즈니스 규칙 표
- Tuner 비즈니스 규칙 표
- 리모컨 버튼 입력 시 주의점 번호 목록
- 채널 0~99, 검색 결과, 선호 채널, Tuner 예외 등 경계값 조건 표
- Google Test 기준 테스트 시나리오 번호 목록

문서 작성 후 저장 내용을 확인하고 linter 진단을 확인한다.
```

### User

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```

### Assistant

```text
기존 Report와 Prompting 폴더의 파일명 규칙과 문서 형식을 확인한다.

기존 파일:
- Report/01_cursorrules_report.md
- Prompting/01_cursorrules_report-Prompt.md

두 번째 산출물로 다음 파일을 작성한다.
- Report/02_requirements_analysis_report.md
- Prompting/02_requirements_analysis_report-Prompt.md

보고서에는 요구사항 분석의 목적, 분석 범위, 주요 산출 내용, QA 관점 기대 효과, 생성 파일 목록을 포함한다.

프롬프트 파일에는 현재까지의 사용자 요청과 assistant 수행 계획을 대화형 transcript 형식으로 정리한다.
```

## Expected Output Files

- `docs/requirements_analysis.md`
- `Report/02_requirements_analysis_report.md`
- `Prompting/02_requirements_analysis_report-Prompt.md`
