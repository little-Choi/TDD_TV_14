# TDD TV — 결함 관리 문서

| 항목 | 내용 |
|---|---|
| 문서 버전 | 1.0 |
| 작성 기준 | [requirements_analysis.md](requirements_analysis.md), [test_plan.md](test_plan.md) |
| 대상 모듈 | `TVController`, `Tuner`(계약), `remoteKey` |
| 기술 스택 | C++17, Google Test / Google Mock, CMake, gcov, **lcov** |
| 결함 추적 | 이슈 트래커(GitHub Issues 등) + 본 문서 템플릿 |

---

## 1. 목적 및 범위

### 1.1 목적

TDD TV 프로젝트에서 발견되는 **결함(Defect)**을 일관된 기준으로 분류·기록·추적하고, 수정·검증·회귀 방지까지의 흐름을 정의한다. 품질 메트릭(테스트 통과율, 커버리지, 단계별 결함 발견율)을 수집해 [test_plan.md](test_plan.md)의 Exit Criteria 달성 여부를 객관적으로 판단한다.

### 1.2 결함 정의

| 구분 | 정의 | 예시 |
|---|---|---|
| **결함(Defect)** | 요구사항·테스트 기대와 **구현 결과가 불일치**하는 것 | CTL-05 실패: 대기 숫자 `6`이 무효화되지 않음 |
| **테스트 결함** | 테스트 코드·Mock 설정·assertion 오류 | `EXPECT_CALL` 순서·인자 불일치로 인한 False Negative |
| **요구 결함** | 요구사항 모호·상충·누락 | `"07"` 전달 형식 미정(§5.3) |
| **정보(Info)** | 개선 제안·기술 부채·문서 불일치(동작 영향 없음) | 로그 메시지 오타, 주석과 코드 불일치 |

본 문서의 **Severity**는 주로 **제품 결함**에 적용한다. 테스트·요구 결함은 동일 템플릿을 쓰되 `유형` 필드로 구분한다.

---

## 2. 결함 분류 체계

### 2.1 Severity 정의

| Severity | 명칭 | 정의 | 대응 SLA (권장) |
|:---:|---|---|---|
| **S1** | Critical | 핵심 채널 제어 불가, 크래시, 데이터 손상, 보안·안전 이슈 | 즉시 차단·핫픽스 (24h 이내) |
| **S2** | Major | README·P0 요구 위반, Golden Master 불일치, 주요 시나리오 오동작 | 현재 스프린트 내 수정 |
| **S3** | Minor | P1/P2 시나리오·경계값 오동작, UX/로그 문제, 우회 가능 | 백로그·다음 스프린트 |
| **S4** | Info | 문서·리팩터링 제안, 커버리지 미달 경고(기능 정상) | 계획 시 반영 |

### 2.2 Severity × 영향도 × 발생빈도 매트릭스

**영향도(Impact)** — 사용자·시스템에 미치는 범위

| 영향도 | 설명 | TDD TV 예시 |
|---|---|---|
| **높음 (H)** | 채널 변경·검색·선호 등 **핵심 기능 전면 실패** 또는 잘못된 CH로 Tuner 호출 | `setCH` 미호출·잘못된 채널·무한 `seekCH` 루프 |
| **중간 (M)** | 특정 입력 조합·모드에서만 오동작 | 검색 목록 있을 때만 업/다운 오류 |
| **낮음 (L)** | 표시·로그·비핵심 경계만 영향 | `std::cout` 포맷, 단일 P2 케이스 |

**발생빈도(Frequency)** — 재현·노출 가능성

| 발생빈도 | 설명 | TDD TV 예시 |
|---|---|---|
| **항상 (A)** | 동일 입력 시 100% 재현 | `KEY_1`,`KEY_OK` 후 채널 미변경 |
| **자주 (O)** | 일반 사용 시나리오에서 자주 발생 | 2자리 자동 확정 실패 |
| **가끔 (R)** | 특정 상태·순서 필요 | 버퍼 대기 `6` + 특정 비숫자 키 조합 |
| **드묾 (U)** | 극단 경계·Mock 비정상 시퀀스 | `getCurrentCH()` 비숫자 반환 |

**Severity 판정 매트릭스** (교차 셀 = 권장 Severity)

| 영향도 \ 발생빈도 | **A (항상)** | **O (자주)** | **R (가끔)** | **U (드묾)** |
|:---:|:---:|:---:|:---:|:---:|
| **H (높음)** | **Critical** | **Critical** | **Major** | **Major** |
| **M (중간)** | **Major** | **Major** | **Minor** | **Minor** |
| **L (낮음)** | **Minor** | **Minor** | **Info** | **Info** |

> **승격 규칙**: 릴리스 직전·CI `main` 브랜치에서 P0 테스트(CTL-xx, TUN-xx) 실패는 최소 **Major**, Golden Master 실패는 **Major** 이상으로 기록한다.  
> **강등 금지**: Critical/Major는 수정·검증 완료 전까지 Severity를 낮추지 않는다(상태만 `Verified`로 변경).

### 2.3 우선순위·상태·근본 원인 분류

**우선순위 (Priority)** — 수정 순서 (Severity와 독립 조정 가능)

| Priority | 기준 |
|:---:|---|
| P0 | S1/S2 + P0 테스트 ID 연계 |
| P1 | S2/S3 + P1 테스트·경계값 |
| P2 | S3/S4 + P2·회귀·문서 |

**결함 상태 (Status)**

| 상태 | 설명 |
|---|---|
| `New` | 신규 등록 |
| `Assigned` | 담당자 지정 |
| `In Progress` | 수정 중 |
| `Fixed` | 수정 완료, 검증 대기 |
| `Verified` | 수정 검증 완료(테스트 Green) |
| `Closed` | 종료(또는 Won't Fix / Duplicate) |
| `Reopened` | 회귀 또는 검증 실패 |

**근본 원인 (Root Cause Category)** — 통계용

| 코드 | 분류 | 예시 |
|:---:|---|---|
| RC-REQ | 요구 누락·오해 | 버퍼 무효화 시점 미정 |
| RC-LOGIC | 로직 오류 | modulo 순환, `upper_bound` 오용 |
| RC-STATE | 상태 관리 | `processingCH` 미클리어 |
| RC-API | Tuner 계약 위반 | 잘못된 `setCH` 인자 형식 |
| RC-TEST | 테스트·Mock 오류 | `InSequence` 누락 |
| RC-BUILD | 빌드·CI·도구 | gcov/lcov 미생성 |
| RC-REG | 회귀 | 리팩터링 후 Golden 불일치 |

---

## 3. 결함 보고서 템플릿

이슈 트래커에 등록할 때 아래 필드를 **필수**로 채운다. ID 형식: `DEF-YYYY-NNN` (예: `DEF-2026-001`).

---

### DEF-____-___ : [한 줄 요약]

| 필드 | 내용 |
|---|---|
| **ID** | DEF-YYYY-NNN |
| **제목** | |
| **Severity** | Critical / Major / Minor / Info |
| **Priority** | P0 / P1 / P2 |
| **상태** | New / … |
| **유형** | 제품 결함 / 테스트 결함 / 요구 결함 / Info |
| **모듈** | TVController / Tuner / remoteKey / CI·빌드 |
| **관련 요구** | README §n, [requirements_analysis.md](requirements_analysis.md) § |
| **관련 테스트** | TUN-xx, CTL-xx, BND-xx, Golden 시나리오명 |
| **보고자** | |
| **담당자** | |
| **발견 단계** | §4.2 참조 (예: UT-Red, CI, Golden) |
| **환경** | OS, 컴파일러, `cmake` 빌드 타입, 커밋 SHA |
| **근본 원인 코드** | RC-xxx |

#### 3.1 재현 절차 (Reproduction Steps)

1. 빌드: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build`
2. (해당 시) 테스트 실행: `ctest --test-dir build --output-on-failure -R <테스트명>`
3. 입력 시퀀스 / Mock 설정:

```text
전제: getCurrentCH() → "6", 검색 목록 없음
입력: KEY_4, KEY_5, KEY_6, KEY_UP
```

4. 관측: 실패한 assertion, 로그, `setCH` 호출 이력

#### 3.2 기대 결과 (Expected)

- 요구사항·[test_plan.md](test_plan.md) 기준 동작을 **측정 가능하게** 기술한다.

```text
기대: setCH("45") 1회 후, 대기 숫자 "6" 무효화.
      KEY_UP 시 일반 업 동작만 수행 (setCH("7") 등 CH 6 기준 +1).
```

#### 3.3 실제 결과 (Actual)

```text
실제: setCH("45") 후 setCH("6") 호출됨 / 또는 KEY_UP 후에도 버퍼 "6" 유지.
```

- 실패 테스트 출력 붙여넣기:

```text
[  FAILED  ] TVControllerTest.PendingDigit_InvalidatedByNonDigitNonOk
Expected: setCH("45") only ...
```

#### 3.4 원인 분석 (Root Cause)

| 항목 | 내용 |
|---|---|
| **근본 원인** | (예: RC-STATE — 비숫자·비OK 입력 시 `clearPendingDigit()` 미호출) |
| **영향 범위** | 숫자 버퍼 대기 상태 전반 / 특정 키만 |
| **유사 결함** | DEF-____-___ |

#### 3.5 수정 내용 (Fix)

| 항목 | 내용 |
|---|---|
| **수정 요약** | |
| **변경 파일** | `include/TVController.h`, `test/TVControllerTest.cpp`, … |
| **커밋/PR** | #nnn |
| **회귀 테스트** | 신규/강화 `TEST_F` 이름 |

#### 3.6 검증 (Verification)

| 검증 항목 | 결과 | 비고 |
|---|---|:---:|
| 단위 테스트 | `TVControllerTest`, `TunerTest` Green | ☐ |
| Golden Master | `TVControllerGoldenTest` Green | ☐ |
| `ctest` 전체 | 100% pass | ☐ |
| 관련 CTL/BND ID | 해당 케이스 pass | ☐ |
| lcov (해당 모듈) | line ≥ 목표, 분기 미회귀 | ☐ |
| 수동 시나리오 (필요 시) | | ☐ |

**검증 명령 예시**

```bash
cmake --build build
ctest --test-dir build --output-on-failure
# 커버리지 (GNU/Clang, CMakeLists --coverage 활성)
lcov --directory build --zerocounters
ctest --test-dir build --output-on-failure
lcov --directory build --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/googletest/*' '*/gmock/*' '*/test/*' \
  --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory coverage-html
```

**검증자 / 검증 일자**: ___________ / ___________

---

### 3.7 결함 등록 체크리스트 (요약)

- [ ] 재현 절차가 제3자가 따라 할 수 있을 만큼 구체적인가?
- [ ] 기대/실제가 테스트 ID·`EXPECT_CALL`과 연결되는가?
- [ ] Severity 매트릭스(§2.2)에 따라 등급이 정당화되는가?
- [ ] 수정 후 **동일 테스트 + 회귀 스위트**로 Verified 했는가?

---

## 4. 품질 메트릭 수집 계획

### 4.1 수집 주기·책임

| 메트릭 | 수집 시점 | 담당 | 저장 위치 |
|---|---|---|---|
| 테스트 통과율 | 매 커밋·PR, nightly | CI / QA | CI 로그, 대시보드 |
| 코드 커버리지 (lcov) | PR·릴리스 전, weekly | Dev / QA | `coverage-html/`, 아티팩트 |
| 단계별 결함 발견율 | 스프린트 종료, 마일스톤 | QA | §4.4 결함 로그 |
| 결함 밀도·수정률 | 릴리스 후 | QA | §4.5 |

### 4.2 테스트 통과율 (Test Pass Rate)

**정의**

\[
\text{통과율(\%)} = \frac{N_{\text{passed}}}{N_{\text{passed}} + N_{\text{failed}} + N_{\text{error}}} \times 100
\]

**대상 스위트**

| 스위트 | 실행 방법 | 목표 |
|---|---|:---:|
| Tuner 계약 | `TunerTest` | 100% |
| Controller 단위 | `TVControllerTest` | 100% |
| Golden Master | `TVControllerGoldenTest` (LABEL `golden`) | 100% |
| **전체** | `ctest --test-dir build --output-on-failure` | **100%** |

**수집 방법**

```bash
ctest --test-dir build --output-on-failure -j 4
# 요약: build/Testing/Temporary/LastTest.log
# 또는: ctest --test-dir build -N  # 테스트 개수 확인
```

**CI 연동** (`.github/workflows/ci.yml`): `main`/`master` push·PR마다 통과율 100%를 **게이트**로 사용. 실패 시 PR 머지 차단.

**기록 양식 (스프린트별)**

| 일자 | 총 테스트 | Pass | Fail | Error | 통과율(%) | 비고 |
|---|---:|---:|---:|---:|---:|---|
| YYYY-MM-DD | | | | | | |

### 4.3 코드 커버리지 — C++ lcov

프로젝트 `CMakeLists.txt`는 GNU/Clang에서 `--coverage`를 테스트 타깃에 적용한다. **lcov**로 리포트를 생성한다.

**측정 범위**

| 포함 | 제외 |
|---|---|
| `include/TVController.h` 등 자체 구현 TU | `googletest`, `gmock`, 시스템 헤더 |
| Controller 분기·public API | 벤더 `Tuner` 실구현( Mock만 사용) |

**목표** ([test_plan.md](test_plan.md) §6.1)

| 지표 | 목표 |
|---|---:|
| Line coverage | ≥ 90% |
| Branch coverage | ≥ 85% |
| Function coverage | 100% (public API) |

**수집 절차 (Linux / CI 권장)**

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

lcov --directory build --zerocounters
ctest --test-dir build --output-on-failure
lcov --directory build --capture --output-file coverage.info

lcov --remove coverage.info '/usr/*' '*/googletest/*' '*/gmock/*' '*/test/*' \
  --output-file coverage.filtered.info

# 요약 수치 확인
lcov --summary coverage.filtered.info

genhtml coverage.filtered.info --output-directory coverage-html
```

**Windows**: WSL 또는 MSYS2에서 동일 절차 수행. MinGW 빌드 시 `build-mingw` 디렉터리를 `--directory`로 지정.

**메트릭 기록 양식**

| 일자 | Line % | Branch % | Function % | 필터 파일 | 미달 분기(요약) |
|---|---:|---:|---:|---|---|
| YYYY-MM-DD | | | | `coverage.filtered.info` | |

**CI 게이트 (선택)**: line coverage < 88% 시 실패. PR에 `coverage-html` 또는 `lcov` info 아티팩트 업로드.

### 4.4 단계별 결함 발견율 (Defect Detection Rate by Phase)

**개발 단계 정의** (TDD TV)

| 단계 코드 | 단계명 | 설명 | 발견 주체 |
|:---:|---|---|---|
| **REQ** | 요구·분석 | requirements_analysis, README 해석 | PO, QA, Dev |
| **UT-Red** | 단위 테스트 Red | TDD 실패 테스트 작성 | Dev |
| **UT-Green** | 단위 테스트 Green | 구현·Mock 검증 | Dev, QA |
| **UT-Refactor** | 리팩터링 | 구조 변경, Golden 영향 | Dev, QA |
| **BND** | 경계값·예외 | BND-xx, EXC-xx, SPC-xx | QA |
| **GM** | Golden Master | `TVControllerGoldenTest` | QA, CI |
| **CI** | 지속 통합 | GitHub Actions | CI |
| **REL** | 릴리스·회귀 | 태그·배포 후 | QA, 현장 |

**단계별 결함 발견율**

\[
\text{단계 } i \text{ 발견율(\%)} = \frac{D_i}{\sum_j D_j} \times 100
\]

- \(D_i\): 단계 \(i\)에서 **최초 보고**된 결함 건수 (재오픈은 원래 발견 단계에 귀속하거나 별도 `Reopened` 통계)

**목표 프로파일 (참고 — 소프트웨어 공학 일반 경향)**

| 단계 | 이상적 비중 | TDD TV 관점 |
|---|:---:|---|
| REQ | 5–15% | §5.3 미정 항목 조기 발견 |
| UT-Red / UT-Green | 40–60% | TDD로 조기 격리 |
| BND / GM | 15–25% | 경계·시나리오 통합 |
| CI / REL | &lt; 10% | 늦은 발견 최소화 |

**스프린트 결함 로그 (집계용)**

| ID | Severity | 발견 단계 | 테스트 ID | 상태 | 발견일 | 수정일 |
|---|---|---|---|---|---|
| DEF-2026-001 | Major | UT-Green | CTL-05 | Verified | | |

**파생 지표**

| 지표 | 공식 | 용도 |
|---|---|---|
| **조기 발견율** | (REQ + UT-Red + UT-Green) / 전체 | TDD 효과 |
| **테스트 기반 발견율** | (UT-* + BND + GM) / 전체 | 테스트 자산 품질 |
| **CI/릴리스 누출율** | (CI + REL) / 전체 | 게이트 강화 필요 여부 |
| **결함 제거 효율** | Verified / (New + …) | 스프린트 마감 |

### 4.5 추가 품질 지표

| 지표 | 정의 | 목표 |
|---|---|---|
| **결함 밀도** | Verified 결함 수 / KLOC (자체 코드) | 스프린트별 추이 관리 |
| **평균 수정 시간 (MTTR)** | New → Verified 영업일 | S1: 1일, S2: 3일 |
| **재오픈율** | Reopened / Verified | &lt; 5% |
| **P0 결함 잔존** | S1/S2 + P0 테스트 미해결 | 릴리스 전 **0** |
| **Golden 불일치 건수** | GM 단계 결함 | 0 (릴리스 기준) |

### 4.6 대시보드·보고 (권장)

**주간 QA 요약** (1페이지)

1. `ctest` 통과율 추이
2. lcov line/branch % 및 전주 대비 Δ
3. 단계별 신규 결함 막대图 (REQ ~ REL)
4. Severity별 오픈 결함 수
5. Top 3 위험: P0 미해결, 커버리지 미달 파일, Reopened

**릴리스 Exit Check** ([test_plan.md](test_plan.md) §8 연계)

- [ ] P0 `TEST_F` 전부 Green
- [ ] `ctest` 100%
- [ ] lcov line ≥ 90% (필터 후)
- [ ] S1/S2 오픈 결함 0
- [ ] §5.3 미정 요구 합의·테스트 반영

---

## 5. 결함 ↔ 테스트 ID 추적

| 결함 유형 | 연계 문서 | 등록 시 필수 링크 |
|---|---|---|
| 채널 입력·버퍼 | CTL-01 ~ CTL-06, BND-15 ~ 18 | test_plan §3.2 |
| 업/다운 | CTL-17 ~ 21, BND-09 ~ 14 | test_plan §3.6 |
| 선호 채널 | CTL-07 ~ 13, BND-20 ~ 22 | test_plan §3.3, §3.4 |
| 검색 | CTL-14 ~ 16, BND-19 | test_plan §3.5 |
| Tuner 계약 | TUN-01 ~ 05, EXC-01 ~ 03 | requirements §3, test_plan §5.1 |
| Golden 회귀 | Golden 시나리오 파일명 | `test/golden/approved/` |

---

## 6. 변경 이력

| 버전 | 일자 | 변경 내용 |
|---|---|---|
| 1.0 | 2026-05-19 | 초안: Severity 매트릭스, 결함 템플릿, lcov 메트릭 계획 |
