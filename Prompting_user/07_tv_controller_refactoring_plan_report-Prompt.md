# 07. TVController 리팩터링 계획 — User Prompts

### User 1

```text
@TVController.h @requirements_analysis.md

[P] 모던 C++ 리팩토링 코치입니다.
[C] 제약: 채널 0~99, 테스트 Green에서만 진행
[T] class TVController 리팩토링 계획을 단계별로 제안해줘.
    - 조건 분기 축소/중복 제거 (커밋 단위로 쪼개기)
    - 타입/정책 분리 (전략 패턴/테이블 기반/함수 분해)
    - 매직 넘버 상수화와 C++17 스타일 개선
[F] 단계별 체크리스트 + 각 단계 검증 방법 (cmake --build && ctest)
```

### User 2

```text
Phase별 내용 전부 REAME.md 파일에 정리하여 추가해줘.
```

### User 3

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```
