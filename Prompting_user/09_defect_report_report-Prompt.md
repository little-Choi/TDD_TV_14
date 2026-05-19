# 09. 결함 관리 — User Prompts

### User 1

```text
@docs/requirements_analysis.md @docs/test_plan.md

[P] QA 리드 엔지니어입니다.
[T] 결함 관리 문서를 defect_report.md로 작성해줘.
    1) 결함 분류 체계:
       Severity (Critical/Major/Minor/Info) 매트릭스 표
    2) 결함 보고서 템플릿 (재현/기대/실제/원인/수정/검증)
    3) 품질 메트릭 수집 계획
       - 테스트 통과율, 커버리지, 단계별 결함 발견율
       - C++: lcov
[F] Markdown. docs 폴더에 defect_report.md로 저장
```

### User 2

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```
