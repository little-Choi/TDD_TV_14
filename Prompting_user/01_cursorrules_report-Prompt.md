# 01. Cursor Rules — User Prompts

### User 1

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

### User 2

```text
@.cursorrules
아래 코드 규칙 추가해줘.
코드 규칙: 테스트메서드명은 should_[결과]_when_[조건] 형태, 한 함수는 최대 20줄
```

### User 3

```text
Report 폴더에 파일번호를 매겨서 보고서 작성해줘.
```

### User 4

```text
현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해줘 파일명은 보고서 파일명에 -Prompt를 붙여줘.
```
