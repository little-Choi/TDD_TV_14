# 09. 결함 관리 보고서

## 개요

- 대상 프로젝트: TDD TV C++17 프로젝트
- 작성 파일: `docs/defect_report.md`
- 목적: 요구사항 분석·테스트 계획을 기준으로 결함 분류·보고·품질 메트릭 수집 체계 수립
- 작성일: 2026-05-19
- 작성 역할: QA 리드 엔지니어 관점

## 작성 범위

`docs/requirements_analysis.md`의 비즈니스 규칙·경계값·예외 조건과 `docs/test_plan.md`의 테스트 ID(CTL/BND/TUN/EXC/SPC), Exit Criteria, gcov/lcov 전략을 결함 관리와 연계했다. 대상 모듈은 `TVController`, `Tuner`(계약), `remoteKey`이며, 기존 `CMakeLists.txt`의 `--coverage` 설정 및 CI(`ctest`)와 메트릭 수집 절차를 반영했다.

## 주요 산출 내용

### 결함 분류 체계 (§2)

- **Severity 4단계**: Critical / Major / Minor / Info (S1~S4) 및 SLA 권장
- **영향도(H/M/L) × 발생빈도(A/O/R/U) 매트릭스**로 Severity 판정
- P0 테스트·Golden Master 실패 시 승격 규칙, Critical/Major 강등 금지
- 결함 유형: 제품 / 테스트 / 요구 / Info
- 상태(New → Verified/Closed), Priority(P0~P2), 근본 원인 코드(RC-REQ ~ RC-REG)

### 결함 보고서 템플릿 (§3)

- ID 형식 `DEF-YYYY-NNN`, 필수 필드: **재현 / 기대 / 실제 / 원인 / 수정 / 검증**
- CTL·BND·TUN·Golden 시나리오와 요구 문서 § 연계
- `ctest`·lcov 검증 명령 및 체크리스트(Verified 전 Severity 강등 금지)

### 품질 메트릭 수집 계획 (§4)

| 메트릭 | 목표·방법 |
|---|---|
| **테스트 통과율** | `TunerTest`, `TVControllerTest`, `TVControllerGoldenTest` 포함 `ctest` **100%** |
| **코드 커버리지** | lcov: line ≥ 90%, branch ≥ 85%, function 100% (test_plan §6.1) |
| **단계별 결함 발견율** | REQ → UT-Red/Green → BND → GM → CI → REL 단계별 \(D_i\) 집계 |
| **파생 지표** | 조기 발견율, CI 누출율, MTTR, 재오픈율, P0 잔존 결함 |

- GNU/Clang `--coverage` + `lcov --capture` / 필터(googletest·gmock·test 제외) / `genhtml`
- 스프린트별 통과율·커버리지·결함 로그 기록 양식
- 릴리스 Exit Check: test_plan §8과 연동

### 결함 ↔ 테스트 추적 (§5)

- 숫자 버퍼(CTL-01~06), 업/다운(CTL-17~21), 선호·검색, Tuner 계약(TUN), Golden 회귀를 문서·테스트 ID로 1:1 추적

## QA 관점 기대 효과

- 결함 등급을 주관이 아닌 **매트릭스**로 통일해 우선순위 논쟁을 줄인다.
- TDD 단계(UT-Red/Green)에서 조기 발견율을 측정해 테스트 자산 품질을 관리할 수 있다.
- test_plan의 Exit Criteria와 결함·커버리지 게이트를 동일 문서 체계로 연결한다.
- 이슈 트래커와 `DEF-*` 템플릿으로 수정·검증·회귀 이력을 감사 가능하게 남긴다.

## 생성 파일

- `docs/defect_report.md`
- `Report/09_defect_report_report.md`
- `Prompting/09_defect_report_report-Prompt.md`
