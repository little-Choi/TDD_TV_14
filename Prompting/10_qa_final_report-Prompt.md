# 10. QA 종합 최종 대화형 프롬프트

## 목적

TDD TV C++17 프로젝트의 요구사항·테스트 계획·결함 관리·구현·테스트 코드를 기준으로 QA 리드 관점의 **종합 QA 최종 보고서**를 작성하고, 해당 산출물에 대한 보고서와 대화형 프롬프트 transcript를 기록한다.

## Conversation Transcript

### User

```text
@requirements_analysis.md @test_plan.md @defect_report.md
@ChannelPolicy.h @DigitInputBuffer.h @remoteKey.h @Tuner.h @TVController.h
@TunerTest.cpp @TVControllerGoldenTest.cpp @TVControllerTest.cpp

[P] QA 리드 엔지니어 관점에서
[T] TDD TV C++ 프로젝트의 QA 활동을 종합 검토해줘.
    1) 테스트 완료율/커버리지 (목표 대비 lcov 수치)
    2) 결함 패턴 분석 (아이템 타입별/심각도별)
    3) 9단계 중 효과적이었던 단계 & 개선 필요 단계
    4) 다음 레거시 프로젝트를 위한 Best Practice 5가지
    5) Cursor AI 활용 효과 (시간 단축/결함 조기 발견/커버리지 향상) 정량·정성 요약
[F] Markdown 최종 보고서. docs폴더에 qa_final_report.md로 저장
```

### Assistant

```text
docs/requirements_analysis.md, docs/test_plan.md, docs/defect_report.md,
include/*, test/*, Report/01~09, CMakeLists.txt, .github/workflows/ci.yml를 확인한다.

ctest 실행: 77/77 Green (TunerTest 13 + TVControllerTest 32 + Golden 32).

lcov 실측 (MinGW g++ 15.2.0, Chocolatey lcov):
- ctest 후 lcov --capture → googletest/gmock/test/_deps 제외
- lcov --extract "*TDD_TV_14/include/*" → build/coverage.tddtv.info
- Line 97.6% (163/167), Function 100% (31/31), Branch 미측정

docs/qa_final_report.md 작성. 포함:
§1 Executive Summary
§2 테스트 완료율 (ctest 100%, CTL/TUN/BND ID 매핑, lcov 목표 대비)
§3 결함 패턴 (재구성 13건, Severity×유형, RC 분포, 단계별 발견율)
§4 9단계 효과 (Report 01~09, Top3 효과/개선)
§5 Best Practice 5가지
§6 Cursor AI 정량·정성
§7 Exit Criteria 체크리스트 → 조건부 릴리스 가능
§8 후속 조치 P0~P2
```

### User

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```

### Assistant

```text
기존 Report·Prompting 폴더의 파일명 규칙과 문서 형식을 확인한다.

기존 파일:
- Report/01_cursorrules_report.md ~ Report/09_defect_report_report.md
- Prompting/*-Prompt.md (동일 번호·베이스명)

다음 산출물로 작성한다.
- Report/10_qa_final_report.md
- Prompting/10_qa_final_report-Prompt.md

보고서에는 QA 종합 검토의 목적, 테스트·lcov 결과, 결함 패턴,
9단계 효과, Best Practice, Cursor AI 효과, Exit 판정, 생성 파일 목록을 포함한다.

프롬프트 파일에는 QA 종합 검토 요청부터
Report/Prompting export 요청까지의 대화형 transcript를 정리한다.
```

## Expected Output Files

- `docs/qa_final_report.md`
- `Report/10_qa_final_report.md`
- `Prompting/10_qa_final_report-Prompt.md`
