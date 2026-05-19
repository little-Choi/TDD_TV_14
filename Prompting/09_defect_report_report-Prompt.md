# 09. 결함 관리 대화형 프롬프트

## 목적

TDD TV C++17 프로젝트의 요구사항 분석·테스트 계획 문서를 기준으로 QA 리드 관점의 결함 관리 문서를 작성하고, 해당 산출물에 대한 보고서와 대화형 프롬프트 transcript를 기록한다.

## Conversation Transcript

### User

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

### Assistant

```text
docs/requirements_analysis.md, docs/test_plan.md, CMakeLists.txt, CI 워크플로를 확인한다.

docs/defect_report.md를 작성한다. 포함 내용:
§1 목적·범위·결함 정의(제품/테스트/요구/Info)
§2 Severity 4단계, 영향도×발생빈도 매트릭스, 승격·강등 규칙,
    상태·Priority·근본 원인 코드(RC-REQ ~ RC-REG)
§3 DEF-YYYY-NNN 결함 보고서 템플릿
    (재현/기대/실제/원인/수정/검증, ctest·lcov 검증 명령)
§4 품질 메트릭: 통과율, lcov 커버리지(90%/85%), 단계별 결함 발견율,
    조기 발견율·CI 누출율·MTTR, 스프린트 기록 양식, Exit Check
§5 결함↔CTL/BND/TUN/Golden 추적 매핑
```

### User

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```

### Assistant

```text
기존 Report·Prompting 폴더의 파일명 규칙과 문서 형식을 확인한다.

기존 파일:
- Report/01_cursorrules_report.md ~ Report/08_tv_controller_refactoring_implementation_report.md
- Prompting/*-Prompt.md (동일 번호·베이스명)

다음 산출물로 작성한다.
- Report/09_defect_report_report.md
- Prompting/09_defect_report_report-Prompt.md

보고서에는 결함 관리 문서의 목적, 분류 체계, 템플릿, 메트릭 수집 계획,
QA 기대 효과, 생성 파일 목록을 포함한다.

프롬프트 파일에는 결함 관리 문서 작성 요청부터
Report/Prompting export 요청까지의 대화형 transcript를 정리한다.
```

## Expected Output Files

- `docs/defect_report.md`
- `Report/09_defect_report_report.md`
- `Prompting/09_defect_report_report-Prompt.md`
