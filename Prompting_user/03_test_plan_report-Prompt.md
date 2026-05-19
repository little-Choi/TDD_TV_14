# 03. 테스트 계획 — User Prompts

### User 1

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

### User 2

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```
