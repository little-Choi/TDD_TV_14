# 07. TVController 모던 C++ 리팩토링 계획 대화형 프롬프트

## 목적

TDD TV C++17 프로젝트에서 Green 상태인 `TVController`에 대해 **동작 변경 없는** 모던 C++ 리팩토링 계획(Phase 0~9+)을 수립하고 `README.md`에 문서화한 뒤, 해당 산출물에 대한 보고서와 대화형 프롬프트 transcript를 기록한다.

## Conversation Transcript

### User

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

### Assistant

```text
TVController.h, requirements_analysis.md, TVControllerTest.cpp, CMakeLists.txt를 확인한다.

현재 77/77 테스트 Green(TunerTest 13 + TVControllerTest 32 + TVControllerGoldenTest 32)을 확인한다.

header-only TVController의 핫스팟을 분석한다.
- 매직 넘버 99/0/2/-1
- handleDigit/handleOk 중복
- channelUp/Down 검색 유무 이중 분기
- performSearch 중첩 if
- pushButton switch + early-return
- navigationBase 센티널

Phase 0~9+ 단계별 계획을 제안한다.
- Phase 0: 베이스라인 77/77
- Phase 1: ChannelPolicy constexpr 상수화
- Phase 2: stepLinearChannel
- Phase 3: DigitInputBuffer
- Phase 4: performSearch predicate 분해
- Phase 5: navigateSearchList modulo 통합
- Phase 6: IChannelStepPolicy (핵심, navigationBase 주의)
- Phase 7: pushButton 핸들러 테이블
- Phase 8: optional, 토글, 파싱 단일화
- Phase 9+: Channel 강타입·cpp 분리·유효성(새 TDD)

각 Phase마다 커밋 단위, 대표 검증 테스트, cmake --build && ctest 명령,
리스크, Phase 3+6 동시 커밋 금지, Golden cout 주의, 실패 진단표, C++17 체크리스트를 포함한다.
```

### User

```text
Phase별 내용 전부 REAME.md 파일에 정리하여 추가해줘.
```

### Assistant

```text
README.md 구조를 확인한다(Golden Master 절 이후).

「TVController 모던 C++ 리팩토링 계획」섹션을 파일 끝에 추가한다.
- 제약 및 원칙(0~99, Green 후 진행, Phase 9+ 범위)
- 현황 요약 표
- 공통 검증 명령(PowerShell)
- Phase 0~9+ 전체: 목표, 작업, 코드 스케치, 검증 테스트, 체크리스트, 리스크
- Phase 요약 체크리스트 표
- mermaid 의존성图
- 실패 시 빠른 진단 표
- C++17 스타일 체크리스트

README.md 204행 → 589행(리팩토링 계획 약 387행 추가).
```

### User

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```

### Assistant

```text
기존 Report·Prompting 폴더의 파일명 규칙(01~06)과 문서 형식을 확인한다.

다음 파일을 작성한다.
- Report/07_tv_controller_refactoring_plan_report.md
- Prompting/07_tv_controller_refactoring_plan_report-Prompt.md

보고서에는 리팩토링 계획 목적·범위·TVController.h 현황 분석·
77/77 베이스라인·Phase 0~9+ 요약·체크리스트·mermaid·진단표·
README 반영 내용·기대 효과·후속 작업(Phase 1 적용)·생성 파일 목록을 포함한다.

프롬프트 파일에는 리팩토링 계획 요청 → README 추가 → Report/Prompting export까지의
대화형 transcript를 정리한다.
```

## Expected Output Files

- `README.md` (「TVController 모던 C++ 리팩토링 계획」절)
- `Report/07_tv_controller_refactoring_plan_report.md`
- `Prompting/07_tv_controller_refactoring_plan_report-Prompt.md`

## 재사용 프롬프트 (요약)

다음 P/C/T/F 블록으로 동일 작업을 재현할 수 있다.

```text
@TVController.h @requirements_analysis.md @README.md

[P] 모던 C++ 리팩토링 코치입니다.
[C] 채널 0~99, 각 Phase 후 ctest 77/77 Green, 동작 변경 없음
[T] TVController 리팩토링 Phase N을 구현해줘 (README 리팩토링 계획 절 참조)
[F] cmake --build build && ctest --test-dir build 통과
```
