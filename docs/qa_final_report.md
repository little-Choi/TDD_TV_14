# TDD TV C++ 프로젝트 — QA 종합 최종 보고서

| 항목 | 내용 |
|---|---|
| 문서 버전 | 1.0 |
| 작성일 | 2026-05-19 |
| 작성 역할 | QA 리드 엔지니어 |
| 대상 프로젝트 | TDD TV C++17 (CMake, Google Test/Mock, gcov/lcov) |
| 기준 문서 | [requirements_analysis.md](requirements_analysis.md), [test_plan.md](test_plan.md), [defect_report.md](defect_report.md) |
| 검증 환경 | Windows 10, MinGW g++ 15.2.0, `build/` Debug, `ctest` 77/77 Green |

---

## 1. Executive Summary

TDD TV 프로젝트는 **9단계 QA·개발 파이프라인**(Cursor Rules → 요구분석 → 테스트계획 → Red 테스트 → Green 구현 → Golden Master → 리팩터링 계획·구현 → 결함관리)을 거쳐 **실행 테스트 77/77(100%)**을 달성했다. 자체 헤더(`include/*`) 기준 **Line 97.6%**, **Function 100%**로 [test_plan.md](test_plan.md) §6.1 목표(90%/100%)를 상회한다.

**강점**: P0 시나리오 전면 Green, Mock 단위 + Golden 이중 회귀망, TDD Red 단계의 조기 명세 고정.  
**개선 필요**: Branch 커버리지 미측정(MinGW), EXC/SPC·P2 테스트 ID 잔여, 결함 로그(`DEF-*`) 실측 미착수, CI에 lcov 게이트 미연동.

---

## 2. 테스트 완료율 및 커버리지

### 2.1 실행 테스트 통과율 (`ctest`)

| 스위트 | 건수 | Pass | Fail | 통과율 | 목표 | 판정 |
|---|---:|---:|---:|---:|---:|:---:|
| `TunerTest` | 13 | 13 | 0 | 100% | 100% | ✅ |
| `TVControllerTest` | 32 | 32 | 0 | 100% | 100% | ✅ |
| `TVControllerGoldenTest` | 32 | 32 | 0 | 100% | 100% | ✅ |
| **합계** | **77** | **77** | **0** | **100%** | **100%** | ✅ |

측정 명령:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

CI([`.github/workflows/ci.yml`](../.github/workflows/ci.yml))는 Ubuntu에서 동일 `ctest`를 실행하며, Golden 32건(`LABEL golden`) 포함 전체 스위트를 게이트로 사용한다.

### 2.2 테스트 계획 ID 대비 완료율

[test_plan.md](test_plan.md)의 추적 ID 기준으로 **실행 가능한 P0 항목은 100% Green**이며, P1/P2·예외·특이 케이스는 일부 잔여한다.

#### CTL (Controller) — 21 ID

| 우선순위 | 계획 | 구현·Green | 비고 |
|:---:|---:|---:|---|
| P0 | 16 | 16 | CTL-01~08, 10~11, 14, 17~21 — `TVControllerTest` + Golden 1:1 |
| P1 | 4 | 3 | CTL-12 ✅, CTL-15 ✅, CTL-09 △(다중 선호는 `should_keep_other_favorites_*`로 부분), CTL-16 △(검색 종료는 시나리오 테스트로 암시적) |
| P2 | 1 | 0 | CTL-13(선호 1개 wrap) 미구현 |
| **합계** | **21** | **19~20** | **완료율 약 90~95%** |

P0 CTL ↔ `TEST_F` 매핑 예:

| ID | 테스트명 |
|---|---|
| CTL-01 | `should_set_channel_1_when_digit_1_and_ok` |
| CTL-05 | `should_confirm_45_only_when_key_up_after_digits_4_5_6` |
| CTL-17 | `should_go_to_7_and_5_when_up_and_down_from_6_without_search` |
| CTL-20 | `should_go_to_14_and_4_when_up_down_from_6_on_search_list` |

#### TUN (Tuner 계약) — 5 ID

| ID | 상태 | 근거 |
|---|---|:---:|
| TUN-01 ~ TUN-05 | ✅ 전부 Green | `TunerTest.cpp` — `initChannel`, `TEST_P` 유효/무효 `setCH`, `seekCH` 반복 |

#### BND (경계값) — 22 ID

| 범주 | 완료 추정 | 대표 커버 |
|---|---:|---|
| 채널 0/99·순환 (BND-01~02, 09~14) | ✅ | 업/다운·선호·검색 테스트 |
| 입력 버퍼 (BND-15~18) | ✅ | CTL-03~05, `99` 입력 |
| 검색·선호 (BND-19~22) | ✅~△ | 중복 제거, 빈 선호, wrap — BND-21(선호 1개)는 CTL-13 미구현과 연동 |
| 무효 채널 100+/음수 (BND-07~08) | △ | **Tuner Mock만** — Controller 차단 테스트 없음(Phase 9+ 예정) |
| **합계** | **~16/22 (73%)** | README 경계는 실사용 시나리오로 대부분 충족 |

#### EXC / SPC — 12 ID

| 범주 | 완료 | 비고 |
|---|---:|---|
| EXC-01~03 | 1/3 | Tuner `invalid_argument` Mock만 — Controller 예외 정책 미고정 |
| SPC-01~09 | 0~1/9 | `KEY_OK` 단독, 모드 충돌 등 **명시 테스트 없음** |
| test_plan §5.3 미정 4항 | 0/4 합의 | `"07"` 형식은 구현상 `"7"`로 확정·테스트 Green, 문서 합의는 미완 |

#### 종합 완료율 요약

| 구분 | 완료율 | 판정 |
|---|---:|:---:|
| **실행 테스트 (`ctest`)** | **100%** (77/77) | ✅ Exit Criteria 충족 |
| **P0 테스트 ID** | **100%** (TUN 5 + P0 CTL 16) | ✅ |
| **전체 테스트 계획 ID** | **약 75~80%** | △ P1/P2·EXC·SPC 잔여 |
| **README Test To-Do** | **100%** (77/77 `[x]`) | ✅ |

### 2.3 lcov 코드 커버리지 (목표 대비)

**측정 일자**: 2026-05-19  
**절차**: `ctest` 실행 후 `lcov --capture` → googletest/gmock/test 제외 → `include/*` 추출

```powershell
ctest --test-dir build --output-on-failure
lcov --directory build --capture --output-file build/coverage.info
lcov --remove build/coverage.info "*/googletest/*" "*/gmock/*" "*/test/*" "*/_deps/*" `
  --output-file build/coverage.filtered.info
lcov --extract build/coverage.filtered.info "*TDD_TV_14/include/*" `
  --output-file build/coverage.tddtv.info
lcov --summary build/coverage.tddtv.info
```

#### 목표 대비 결과 ([test_plan.md](test_plan.md) §6.1)

| 지표 | 목표 | 실측 (`include/*`) | Δ | 판정 |
|---|---:|---:|---:|:---:|
| **Line coverage** | ≥ 90% | **97.6%** (163/167 lines) | +7.6%p | ✅ |
| **Branch coverage** | ≥ 85% | **측정 불가** (MinGW gcov branch data 없음) | — | ⚠️ |
| **Function coverage** | 100% (public API) | **100%** (31/31) | 0 | ✅ |

#### 파일별 Line 커버리지

| 파일 | Line % | Function % | 비고 |
|---|---:|---:|---|
| `TVController.h` | **99.3%** (136/137) | 100% | 핵심 비즈니스 로직 |
| `DigitInputBuffer.h` | 90.9% (11/12) | 100% | `empty()` 등 소수 분기 |
| `remoteKey.h` | 89.5% (19/21) | 100% | `to_string` 일부 분기 |
| `ChannelPolicy.h` | N/A | N/A | `constexpr`만 — 실행 라인 없음 |
| `Tuner.h` | 100% (1/1) | 100% | 인터페이스 |

**Exit Criteria 판정**: Line·Function 목표 **달성**. Branch는 **도구·CI 한계로 미평가** — Linux/Clang CI + `--rc branch_coverage=1` 도입 권장.

**측정 한계 (RC-BUILD)**:

- Header-only 구현으로 커버리지가 테스트 TU에 귀속 — `TVController.h` 수정 후 **재빌드 없이** 측정 시 *source newer than notes* 경고 발생 가능.
- Windows MinGW 빌드에서 **branch 라인 미수집** — 목표 85% 검증 불가.

---

## 3. 결함 패턴 분석

본 프로젝트는 9단계 산출물·구현 보고서·TDD 이력을 기반으로 결함을 역추적했다. [defect_report.md](defect_report.md) §3의 `DEF-*` 템플릿은 수립되었으나, **이슈 트래커에 formal `DEF-2026-NNN` 등록은 미실시** — 아래는 **재구성 결함 로그**이다.

### 3.1 Severity × 유형 매트릭스

| 유형 \ Severity | S1 Critical | S2 Major | S3 Minor | S4 Info | 소계 |
|---|:---:|:---:|:---:|:---:|:---:|
| **제품 결함** | 0 | 3 | 2 | 0 | **5** |
| **테스트 결함** | 0 | 0 | 1 | 0 | **1** |
| **요구 결함** | 0 | 0 | 0 | 4 | **4** |
| **정보(Info)** | 0 | 0 | 0 | 3 | **3** |
| **소계** | **0** | **3** | **3** | **7** | **13** |

### 3.2 제품 결함 상세 (대표)

| ID (재구성) | Severity | 발견 단계 | RC | 테스트 ID | 요약 | 상태 |
|---|---|---|---|---|---|:---:|
| DEF-R01 | S2 Major | UT-Green | RC-STATE | CTL-05 | `4,5,6` 후 `UP` 시 대기 `6` 미무효화·잘못된 `setCH` | Verified |
| DEF-R02 | S2 Major | UT-Green | RC-STATE | CTL-17 | 연속 UP/DOWN 시 Mock 미갱신 — `navigationBase` 필요 | Verified |
| DEF-R03 | S2 Major | UT-Green | RC-LOGIC | CTL-14~16 | 검색 종료·중복·무한 루프 방지 로직 | Verified |
| DEF-R04 | S3 Minor | UT-Green | RC-LOGIC | CTL-20~21 | 검색 목록 밖 채널 UP/DOWN `front`/`back` | Verified |
| DEF-R05 | S3 Minor | UT-Refactor | RC-STATE | CTL-05 | Phase 6 리팩터링 시 `456+UP`·`navigationBase_` 타이밍 회귀 위험(사전 예방) | Verified |

### 3.3 테스트·요구·빌드 결함

| ID | 유형 | Severity | 단계 | 요약 |
|---|---|---|---|---|
| DEF-R06 | 테스트 | S3 | UT-Green | `should_remove_favorite_*` assertion 순서·주석 불일치 (RC-TEST) |
| DEF-R07~10 | 요구 | S4 | REQ | §5.3 — `"07"` 표기, 빈 선호 NEXT, Tuner 예외 흡수, 검색 100회 cap (RC-REQ) |
| DEF-R11 | 정보 | S4 | CI/BUILD | lcov branch 미수집·header-only 측정 복잡도 (RC-BUILD) |
| DEF-R12 | 정보 | S4 | REL | Phase 9+ — Controller 무효 채널(100+) 미차단 (RC-REQ, 계획됨) |
| DEF-R13 | 정보 | S4 | — | Golden 한국어 stdout 의존 — 로그 변경 시 baseline 전량 갱신 |

### 3.4 근본 원인(RC) 분포

```
RC-STATE  ████████░░  38%  (5건)  — 버퍼·navigationBase
RC-LOGIC  █████░░░░░  23%  (3건)  — 검색·목록 네비
RC-REQ    ██████░░░░  31%  (4건)  — 미정 요구
RC-TEST   █░░░░░░░░░   8%  (1건)  — 테스트 순서
RC-BUILD  █░░░░░░░░░   8%  (1건)  — lcov/도구
```

### 3.5 단계별 결함 발견율

| 단계 코드 | 단계명 | 건수 | 비율 | 이상적 비중 | 평가 |
|:---:|---|---:|---:|---:|---|
| REQ | 요구·분석 | 4 | 31% | 5~15% | △ 다소 높음(사전 합의 필요) |
| UT-Red | 단위 Red | 1 | 8% | — | ✅ 테스트 설계 단계 격리 |
| UT-Green | 단위 Green | 5 | 38% | 40~60% | ✅ TDD 핵심 구간 |
| UT-Refactor | 리팩터링 | 1 | 8% | — | ✅ Golden으로 회귀 없음 |
| BND | 경계값 | 1 | 8% | 15~25% | ✅ 테스트에 흡수 |
| GM | Golden Master | 0 | 0% | — | ✅ 2차 망에서 신규 0건 |
| CI | 지속 통합 | 0 | 0% | <10% | ✅ |
| REL | 릴리스 | 0 | 0% | <10% | ✅ |

**파생 지표**:

| 지표 | 값 | 해석 |
|---|---:|---|
| 조기 발견율 (REQ+UT-Red+UT-Green) | **77%** | TDD·요구 분석 효과 양호 |
| 테스트 기반 발견율 (UT-*+BND+GM) | **54%** | GM 단계 신규 결함 0 — 1차 테스트 품질 양호 |
| CI/릴리스 누출율 | **0%** | 게이트 유효 |
| 재오픈율 | **0%** (기록 기준) | — |
| P0 잔존 | **0** | 릴리스 가능 수준 |

---

## 4. 9단계 효과 분석

프로젝트 QA 파이프라인은 `Report/01` ~ `Report/09` 및 대응 `Prompting/*` 단계와 1:1 대응한다.

| 단계 | 산출물 | 효과 등급 | 핵심 성과 | 개선 필요 |
|:---:|---|:---:|---|---|
| **01** | `.cursorrules` | ★★★★☆ | 채널 0~99·`should_*_when_*`·Green 후 리팩터링 원칙 고정 | 커버리지/lcov 실행 규칙을 CI 경로까지 구체화 |
| **02** | `requirements_analysis.md` | ★★★★☆ | 22개 GTest 시나리오·버퍼 무효화 등 결함 다발 구간 사전 식별 | §5.3 미정 4건 **합의 워크숍** 미실시 |
| **03** | `test_plan.md` | ★★★★☆ | CTL/BND/TUN ID·Exit Criteria·lcov 전략 | EXC/SPC·P2 ID → `TEST_F` 미전환 |
| **04** | `TVControllerTest` Red | ★★★★★ | 32 Red로 README §1~6 계약 고정 | — |
| **05** | `TVController` Green | ★★★★★ | 32/32 Green, 핵심 제품 결함 5건 해결 | — |
| **06** | Golden Master | ★★★★★ | 32 approved + CI, Mock과 독립 2차 망 | stdout 한글 의존·diff 가독성 |
| **07** | 리팩터링 계획 | ★★★☆☆ | Phase 0~9+·실패 진단표 | 계획만 수립 시점 길어짐 — 04~06 직후 병행 권장 |
| **08** | 리팩터링 구현 | ★★★★☆ | Phase 1~8, 77/77 유지, `DigitInputBuffer`·전략 분리 | Phase 일괄 커밋 → 단계별 커밋·lcov diff 권장 |
| **09** | `defect_report.md` | ★★☆☆☆ | Severity 매트릭스·메트릭 양식 | **실측 DEF 로그·주간 대시보드 미가동** |

### 4.1 가장 효과적이었던 단계 (Top 3)

1. **04단계 — Red 테스트 설계**: 구현 전 `CTL-05`(버퍼 무효화)·0/99 경계를 고정해 UT-Green에서 수정 비용을 최소화했다.  
2. **06단계 — Golden Master**: 리팩터링(08)·`navigationBase` 변경에도 **신규 결함 0건** — 출력·시나리오 회귀를 일괄 감지한다.  
3. **05단계 — Green TDD 구현**: 제품 결함의 80% 이상을 이 단계에서 격리·해결했다.

### 4.2 개선이 필요한 단계 (Top 3)

1. **09단계 — 결함 관리**: 프로세스 문서만 있고 **DEF 등록·Verified 체크리스트 실적 없음** → 02단계부터 경량 로그 병행 필요.  
2. **03단계 — 테스트 계획**: EXC/SPC·Controller 무효 채널(BND-07~08) **실행 테스트 미연결**.  
3. **CI/lcov 게이트**: `CMakeLists.txt`에 `--coverage` 있으나 **CI workflow에 lcov·branch 게이트 없음** — Exit Criteria half-met.

---

## 5. 다음 레거시 프로젝트 Best Practice 5가지

### BP-1. README → 요구분석 → P0 테스트 ID를 같은 스프린트에 묶는다

레거시 README만으로 구현에 들어가면 버퍼·순환·검색 모드 같은 **결함 다발 구간**이 늦게 드러난다. `requirements_analysis.md`로 규칙을 쪼개고, **P0 CTL ID당 최소 1개 `TEST_F`**를 Red로 먼저 둔다.

### BP-2. Mock 계약 테스트 + Golden(Approval) 이중 안전망

`TVControllerTest`(호출 계약)와 `TVControllerGoldenTest`(관찰 가능 출력)를 **동일 시나리오명**으로 유지했다. 리팩터링·로그 변경 시 Golden이 포착, API 계약은 Mock이 포착한다.

### BP-3. AI·인간 공통 규칙 파일(`.cursorrules`)을 1단계에 고정한다

채널 0~99, 함수 20줄, Green 후 리팩터링, Git 보호 규칙을 명문화해 **Cursor AI·수동 개발의 동일 기준**을 확보한다.

### BP-4. 리팩터링은 Phase + `ctest` 게이트, 한 커밋에 버퍼+전략 합치지 않는다

Phase 3(`DigitInputBuffer`)와 Phase 6(`IChannelStepPolicy`)을 분리한 계획이 회귀 원인 분리에 유효했다. 레거시에서도 **동작 불변 리팩터링은 단계별 Green**이 필수다.

### BP-5. 결함·커버리지는 “문서 9단계”가 아니라 2단계부터 측정한다

`DEF-YYYY-NNN`을 REQ 단계 모호 항목부터 등록하고, **첫 Green 직후 lcov 기준선**을 CI 아티팩트로 남긴다. Branch는 Linux CI에서 `--coverage` + lcov branch 옵션으로 85% 게이트를 검증한다.

---

## 6. Cursor AI 활용 효과

### 6.1 정량 요약

| 항목 | 추정·실측 | 비고 |
|---|---|---|
| 프로젝트 일정 | **1일 집중** (보고서·산출물 일자 2026-05-19) | 9단계 문서+코드 동일일 완료 |
| `TEST_F` + Golden | **64** 시나리오 + **77** `ctest` | AI 보조로 대량 생성·동기화 |
| Red → Green 사이클 | **32 실패 → 32 통과** (1 사이클) | 수동 추정 2~3일 → **약 60~70% 단축** |
| 테스트 통과율 | **100%** (77/77) | — |
| Line 커버리지 | **97.6%** (목표 90%) | AI 생성 테스트의 분기 포괄 |
| Golden 회귀 누출 | **0건** (08 리팩터링 후) | — |
| Formal 결함 티켓 | **0건** (프로세스만) | 09단계 human QA 잔여 |

※ 일정·시간 단축률은 동일 역량 수동 작업 대비 **프로젝트 보고서·규모 기반 추정**이며, 공식 공수 데이터는 없다.

### 6.2 정성 요약

| 영역 | 효과 |
|---|---|
| **요구·계획 문서화** | README → `requirements_analysis` / `test_plan` / `defect_report` 자동 구조화. QA 리드가 검토·승인하는 시간은 줄고, **일관된 ID 체계(CTL/BND)** 확보. |
| **테스트 설계** | Given-When-Then·Mock 패턴·경계값(0/99)·`456+UP` 등 **실무 결함 시나리오**를 4단계에서 일괄 설계 — 구현 전 명세 품질 향상. |
| **결함 조기 발견** | CTL-05류 버퍼 버그를 Red 테스트로 **구현 전 포착**; Green 단계에서는 `navigationBase`·검색 종료 등 복합 결함을 테스트 주도로 수정. |
| **리팩터링** | Phase 1~8 일괄 적용 + 77/77 유지 — 인간이면 분기 놓치기 쉬운 업/다운·검색 전략을 **기계적 회귀 검증**에 맡김. |
| **한계** | §5.3 요구 합의·DEF 로그·branch lcov·EXC/SPC는 **human QA 판단** 필요; AI는 “틀린 코드”보다 “정의 안 된 요구”를 스스로 닫지 못함. |

### 6.3 권장 Cursor 활용 패턴 (재현)

1. **컨텍스트 번들**: `@README` + `@requirements_analysis` + `@test_plan` + 헤더/테스트 파일을 단계별로 제공.  
2. **역할 프롬프트**: `[P] QA 리드` / `[P] 시니어 C++ QA`로 산출물 형식 강제 (`Report/0x_*`).  
3. **Red 명시**: 4단계에서 “`ctest` Green이 아니어야 함”을 조건으로 두어 TDD 위반 방지.  
4. **리팩터링 제약**: “동작 변경 없음 + Phase 후 `ctest`”를 매 프롬프트에 반복(08단계).  
5. **종료 검증**: QA 최종 보고서(본 문서)처럼 **lcov 수치·9단계 회고**를 AI 초안 + 인간 승인으로 마감.

---

## 7. Exit Criteria 최종 체크리스트

| 항목 ([test_plan.md](test_plan.md) §8) | 상태 |
|---|:---:|
| P0 `TEST_F` 전부 Green | ✅ |
| BND-01~14 최소 1건 이상 커버 | ✅ (시나리오 테스트에 포함) |
| EXC-01, SPC-05, SPC-09 | ❌ / △ 명시 테스트 없음 |
| `TunerTest` + `TVControllerTest` ctest 100% | ✅ |
| Golden 포함 전체 77/77 | ✅ |
| Line coverage ≥ 90% (lcov 필터 후) | ✅ **97.6%** |
| Branch coverage ≥ 85% | ⚠️ 미측정 |
| S1/S2 오픈 결함 0 | ✅ (재구성 로그 기준 전부 Verified) |
| §5.3 미정 요구 합의 | ❌ 문서화만, 팀 사인오프 없음 |

**종합 판정**: **조건부 릴리스 가능** — 기능·P0·Line/Function 커버리지는 충족. Branch lcov, EXC/SPC, DEF formal tracking, §5.3 합의는 **다음 스프린트 필수 보완**.

---

## 8. 권장 후속 조치 (우선순위)

| P | 조치 | 담당 |
|:---:|---|---|
| P0 | Linux CI job에 `lcov` + line ≥88% 게이트 + branch 수집 | DevOps |
| P0 | `DEF-2026-*` 실제 등록(§5.3 4건 → REQ 결함 Closed) | QA |
| P1 | EXC-01 Controller 정책 테스트 + Phase 9+ 무효 채널 Red | Dev |
| P1 | CTL-13, CTL-16 명시 `TEST_F` | QA/Dev |
| P2 | Golden 실패 시 unified diff (`GoldenMaster.h`) | Dev |

---

## 9. 변경 이력

| 버전 | 일자 | 변경 내용 |
|---|---|---|
| 1.0 | 2026-05-19 | QA 리드 종합 최종 보고서 초판 — lcov 실측(2026-05-19), 9단계 회고, Cursor AI 효과 |

---

## 10. 참고

- 측정 아티팩트: `build/coverage.tddtv.info` (로컬 생성)
- 단계별 보고: `Report/01_cursorrules_report.md` … `Report/09_defect_report_report.md`
- 테스트 목록: `README.md` — Test Case To-Do List (77/77)
