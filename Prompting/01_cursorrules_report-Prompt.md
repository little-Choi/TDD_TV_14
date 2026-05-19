# 01. Cursor Rules 작성 대화형 프롬프트

## 목적

TDD TV C++17 프로젝트에서 Cursor AI가 항상 따라야 하는 `.cursorrules`를 작성하고, 이후 보완 규칙과 보고서 작성까지 이어지는 대화형 프롬프트를 기록한다.

## Conversation Transcript

### User

```text
[P] 당신은 레거시 코드 QA/리팩토링을 돕는 시니어 C++ 엔지니어입니다.
[C] TDD TV C++17 프로젝트에서 Cursor AI가 항상 따라야 하는 규칙을
    프로젝트 루트의 .cursorrules로 작성하려고 합니다.
[T] 아래 요구를 만족하는 .cursorrules 내용을 작성해줘. README.md 파일을 읽고 규칙을 추가해도 돼.
    - 기술 스택: C++17 + CMake + Google Test lcov
    - 절대 규칙: 채널 0~99
    - 테스트 규칙: Given-When-Then, TEST_F, 경계값(채널 0/99) 포함
    - 리팩토링 규칙: 테스트 Green 상태에서만 진행, 매직 넘버 상수화 권장
    - git 규칙: git merge 진행하지 않기
[F] .cursorrules 파일을 만들기
```

### Assistant

```text
README.md를 읽어 프로젝트 요구사항을 확인한 뒤, 프로젝트 루트에 .cursorrules 파일을 생성한다.

규칙에는 다음 항목을 포함한다.
- C++17, CMake, Google Test, lcov 사용
- TV 채널 범위 0~99 절대 준수
- 99에서 채널 업 시 0으로, 0에서 채널 다운 시 99로 래핑
- Tuner는 외부 의존성으로 보고 Fake 또는 Mock 사용
- Given-When-Then 테스트 구조
- TEST_F 사용 권장
- 채널 0과 99 경계값 테스트 포함
- 테스트 Green 상태에서만 리팩토링
- 매직 넘버 상수화 권장
- git merge 금지
```

### User

```text
@.cursorrules
아래 코드 규칙 추가해줘.
코드 규칙: 테스트메서드명은 should_[결과]_when_[조건] 형태, 한 함수는 최대 20줄
```

### Assistant

```text
.cursorrules 파일을 수정한다.

Testing Rules 섹션에 다음 규칙을 추가한다.
- Name test methods in the `should_[result]_when_[condition]` format.

Code Rules 섹션을 추가한다.
- Keep each function at 20 lines or fewer.
```

### User

```text
Report 폴더에 파일번호를 매겨서 보고서 작성해줘.
```

### Assistant

```text
Report 폴더를 확인하고, 없으면 생성한다.

첫 번째 보고서로 `Report/01_cursorrules_report.md`를 작성한다.

보고서에는 다음 내용을 포함한다.
- 개요
- 반영 배경
- 주요 규칙
- 기술 스택
- 도메인 규칙
- 테스트 규칙
- 코드 및 리팩토링 규칙
- Git 규칙
- 기대 효과
```

### User

```text
현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해줘 파일명은 보고서 파일명에 -Prompt를 붙여줘.
```

### Assistant

```text
Prompting 폴더를 확인하고, 없으면 생성한다.

보고서 파일명 `01_cursorrules_report.md`를 기준으로 `-Prompt`를 붙여
`Prompting/01_cursorrules_report-Prompt.md` 파일을 작성한다.

현재까지의 사용자 요청과 assistant 수행 내용을 대화형 transcript 형식으로 정리한다.
```

## Expected Output Files

- `.cursorrules`
- `Report/01_cursorrules_report.md`
- `Prompting/01_cursorrules_report-Prompt.md`
