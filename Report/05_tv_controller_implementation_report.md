# 05. TVController 구현 보고서

## 개요

- 대상 프로젝트: TDD TV C++17 프로젝트
- 작성 파일: `include/TVController.h`, `test/TVControllerTest.cpp` (선호 삭제 테스트 순서 보정)
- 목적: `README.md` §1~§6 동작을 `TVController::pushButton()`에 구현하고 `TVControllerTest` 32건 Green 달성
- 작성일: 2026-05-19
- 작성 역할: 시니어 C++ QA (TDD Green 구현)

## 작업 범위

`04_tv_controller_test_report.md`에서 확보한 Red 상태(32/32 실패)를 기준으로, Mock `Tuner` 계약을 만족하는 Controller 로직을 header-only로 구현했다. `Tuner` 본체·`remoteKey.h` enum은 변경하지 않았다. `std::cout` 로그는 기존 스텁과 동일하게 유지하며 테스트 검증 대상이 아니다.

## 주요 구현 내용

### 1. 숫자 버튼 채널 변경 (README §1)

| 규칙 | 구현 |
|---|---|
| 2자리 즉시 확정 | `processingCH` 길이 ≥ 2 시 `stoi` 후 `setCH` |
| 1자리 + OK 확정 | `handleOk()`에서 버퍼 비어 있지 않으면 확정 |
| 연속 4자리 (예: 1,2,3,4) | 12 → 34 순차 확정 |
| 3자리 (예: 4,5,6) | 45 확정 후 `6` 버퍼 유지, OK 시 6번 확정 |
| 비숫자/비OK 입력 | `clearInputBuffer()`로 대기 숫자 무효화 |
| 앞자리 0 (0,7) | `stoi("07")` → 7 정규화 |

### 2. 선호 채널 (README §2, §3)

- `std::set<int> favorites`로 추가/삭제 토글 (`KEY_FAVORITE_ADD`)
- `upper_bound(current)`로 다음 선호 탐색, 없으면 최소값 순환 (`KEY_FAVORITE_NEXT`)
- 빈 목록 시 `getCurrentCH()` 호출 후 `setCH` 미호출

### 3. 채널 검색 (README §4)

- `KEY_SEARCH` 시 `seekCH()` 반복 호출
- **종료 조건**: (1) 검색 시작 시 `getCurrentCH()`와 동일 채널 재방문, (2) 첫 `seekCH` 결과 재방문 + 결과 2개 이상
- 중복 채널 1회만 저장, 정렬된 `searchResults` 유지
- `getCurrentCH()` 빈 문자열·`seekCH()` 빈 반환 시 예외 없이 종료

### 4. 채널 업/다운 (README §5, §6)

**검색 결과 없음**

| 현재 | UP | DOWN |
|---:|---:|---:|
| 6 | 7 | 5 |
| 99 | 0 (순환) | 98 |
| 0 | 1 | 99 (순환) |

**검색 결과 있음** (`{4,6,14}` 등)

- 목록 내: 인덱스 기준 다음/이전, 양끝 순환
- 목록 밖 (예: 15): UP → 최소(4), DOWN → 최대(14)

**업 직후 다운 (README 연속 시나리오)**

- `navigationBase`: UP 시점의 `getCurrentCH()` 저장
- DOWN은 `navigationBase` 기준으로 계산 (Mock이 `setCH` 후 채널을 갱신하지 않아도 6→7 후 6→5 동작 보장)

## 테스트 결과 (Green)

| 대상 | 결과 |
|---|---|
| `TunerTest` | 13/13 통과 |
| `TVControllerTest` | **32/32 통과** |
| **전체 `ctest`** | **45/45 통과** |

빌드·실행 예 (MinGW):

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build
ctest --test-dir build -R TVController --output-on-failure
```

## 테스트 보정 (1건)

`should_remove_favorite_when_current_channel_already_in_list`는 주석(등록 시 5→10, 삭제 후 setCH 없음)과 코드 순서가 불일치했다. **등록 확인 → 삭제 → 빈 목록 확인** 순으로 재배치하여 README·주석 의도와 일치시켰다.

## 설계 결정 요약

| 항목 | 결정 |
|---|---|
| 구현 위치 | `include/TVController.h` (CMake이 `TVControllerTest`에 소스 미링크) |
| 자료구조 | `set` 선호, `vector` 검색 목록 |
| 채널 표현 | `setCH(std::to_string(ch))` — Tuner 계약 문자열 |
| 로그 | `setTunerCh` 시 `std::cout` 유지 (테스트 비검증) |

## QA 관점 기대 효과

- README §1~§6과 구현·테스트가 1:1 추적 가능
- Mock 계약 기반 Green으로 Controller 회귀 기준 확보
- 경계값(0/99)·버퍼 무효화·검색 중복·업/다운 분기 등 결함 다발 구간 커버
- `04` Red → `05` Green TDD 사이클 완료

## 생성·수정 파일

- `include/TVController.h` (전면 구현)
- `test/TVControllerTest.cpp` (선호 삭제 테스트 순서 보정)
- `Report/05_tv_controller_implementation_report.md`
- `Prompting/05_tv_controller_implementation_report-Prompt.md`
