# 03. 테스트 계획 보고서

## 개요

- 대상 프로젝트: TDD TV C++17 프로젝트
- 작성 파일: `docs/test_plan.md`
- 목적: `README.md` 동작 설명과 요구사항 분석을 바탕으로 Google Test 기반 단위 테스트 계획 수립
- 작성일: 2026-05-19
- 작성 역할: 시니어 QA 리드 관점

## 계획 범위

`README.md`에 정의된 리모컨 숫자 입력·확정, 선호 채널 추가/다음 선호, 채널 검색, 채널 업/다운(검색 결과 유/무) 동작을 테스트 대상으로 삼았다. `docs/requirements_analysis.md`의 경계값·예외 조건과 기존 `TunerTest.cpp`, `TVControllerTest.cpp`의 Mock 패턴을 반영했다.

Tuner 본체 구현 검증은 제외하고, Controller가 `Tuner` API를 올바른 인자·순서로 호출하는지에 집중한다. `std::cout` 로그는 검증 기준에서 제외한다.

## 주요 산출 내용

### 단위 테스트 범위 및 우선순위

- **P0**: README에 직접 명시된 시나리오 — CTL-01~21 (숫자·선호·검색·업/다운)
- **P1**: 경계값·검색 종료·선호 목록 비어 있음 등 — BND, CTL-12~16
- **P2**: 정책 미정·회귀 보강 — 선호 1개, 모드 충돌

`TEST_F` 기반 Fixture(`MockTunerForController` + `TVControllerTest`) 패턴과 TDD 구현 순서(숫자 버퍼 → 업/다운 → 선호 → 검색)를 문서화했다.

### Tuner 계약 테스트 (기존)

- TUN-01~05: `TunerTest.cpp`에 이미 구현된 initChannel, 유효/무효 `setCH`, `seekCH` 반복 검증을 유지·참조 대상으로 명시

### 경계값 케이스

- 채널 **0**, **99** 및 0↔99 순환 (BND-01~02, BND-09~11)
- `0`,`7` → 7번 정규화 (BND-06, CTL-06)
- 100 이상·음수 채널 무효 (BND-07~08)
- 검색 목록 `{4,6,14}` 기준 on-list/off-list 업·다운 (BND-12~14, CTL-20~21)
- 3자리 숫자 입력 시 2자리 확정 + 1자리 대기·무효화 (BND-15~18)

### 예외·특이 케이스

- `std::invalid_argument`, 빈 `seekCH`, 비숫자 `getCurrentCH` (EXC-01~03)
- 버퍼·검색·선호 모드 충돌, `KEY_OK` 단독, `to_string(KEY_OK)` 버그 주의 (SPC-01~09)
- `"07"` 표기, 선호 비어 있을 때 NEXT, Tuner 예외 흡수 여부 등 팀 합의 필요 항목 (§5.3)

### 커버리지 목표 및 측정 전략

| 지표 | 목표 |
|---|---|
| Line (자체 코드) | ≥ 90% |
| Branch | ≥ 85% |
| Function (public API) | 100% |

- CMake `--coverage` 빌드, `lcov` 수집·필터(googletest/gmock/test 제외), `genhtml` 리포트
- `TVController` 헤더-only 시 `src/` 분리 또는 TU 포함 옵션 안내
- 미커버 분기 → `TEST_F` ID 매핑 후 Red→Green 보완 절차

### 완료 기준 (Exit Criteria)

- P0 `TEST_F` 전부 Green
- 경계값 BND-01~14 최소 1건 이상 커버
- EXC-01, SPC-05, SPC-09 포함
- `ctest` 100% pass, line coverage ≥ 90%

## QA 관점 기대 효과

- README 요구와 테스트 ID(CTL/BND/EXC/SPC)를 1:1로 추적할 수 있다.
- `TEST_F` 우선순위로 TDD 스프린트 백로그를 바로 사용할 수 있다.
- gcov/lcov 파이프라인과 90%+ 목표로 커버리지 회귀를 관리할 수 있다.
- 미정 정책(§5.3)을 구현 전에 합의·테스트 assertion에 반영하도록 유도한다.

## 생성 파일

- `docs/test_plan.md`
- `Report/03_test_plan_report.md`
- `Prompting/03_test_plan_report-Prompt.md`
