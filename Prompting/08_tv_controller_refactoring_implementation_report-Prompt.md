# 08. TVController 모던 C++ 리팩토링 구현 대화형 프롬프트

## 목적

TDD TV C++17 프로젝트에서 `README.md`에 정리된 TVController 모던 C++ 리팩토링 계획(Phase 0~8)을 **동작 변경 없이** 구현하고 77/77 Green을 유지한 뒤, 해당 산출물에 대한 보고서와 대화형 프롬프트 transcript를 기록한다.

## Conversation Transcript

### User

```text
@TVController.h @requirements_analysis.md @README.md

[P] C++ 리팩토링 코드 개발자입니다.
[C] 제약: 채널 0~99, 테스트 Green에서만 진행
[T] REAME.md에 정리된 TVController 모던 C++ 리팩토링 계획을 진행해줘
    - 각 Phase별로 리팩토링 및 테스트 진행
[F] 리팩토링 진행한 최종 코드
```

### Assistant

```text
README.md, TVController.h, requirements_analysis.md, 기존 Report·Prompting 형식을 확인한다.

Phase 0: cmake --build build && ctest → 77/77 Green (Tuner 13 + TVController 32 + Golden 32) 확인.

Phase 1~8 일괄 구현:

Phase 1 — ChannelPolicy.h 신규
- tv::kMinChannel, kMaxChannel, kChannelCount, kMaxDigitLength

Phase 2 — tv::stepLinearChannel(current, delta)
- 0↔99 순환 단일화

Phase 3 — DigitInputBuffer.h 신규
- append, readyToCommit, tryTakeChannel, clear
- commitBufferedChannel() 단일 확정 경로

Phase 4 — SearchScanState, shouldStopSearch()
- unordered_set dedup + sort, seekCH 순서 유지

Phase 5 — enum class NavStep, navStepDelta, modulo 인덱스 통합

Phase 6 — IChannelStepPolicy / LinearStepPolicy / SearchListStepPolicy
- channelUp/Down 정책 선택
- NavigationContext, std::optional<int> navigationBase_

Phase 7 — unordered_map<remoteKey, Handler> dispatchNonDigitKey

Phase 8 — parseTunerChannel(), 토글 find/erase/insert, goToNextFavorite target 변수

TVController.h 전면 리팩토링 (~285행).

빌드·테스트: cmake --build build && ctest --test-dir build
→ 77/77 Green, Golden diff 없음 (setTunerCh cout 문자열 유지).

Phase 9+ (Channel 강타입, cpp 분리, 무효 채널)는 별도 TDD 후 진행으로 남김.
```

### User

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```

### Assistant

```text
기존 Report·Prompting 폴더의 파일명 규칙(01~07)과 문서 형식을 확인한다.

다음 파일을 작성한다.
- Report/08_tv_controller_refactoring_implementation_report.md
- Prompting/08_tv_controller_refactoring_implementation_report-Prompt.md

보고서에는 리팩토링 구현 목적·범위·Phase 0~8별 구현 내용·
아키텍처 mermaid·77/77 테스트 결과·C++17 체크리스트·
생성·수정 파일·설계 결정·Phase 9+ 후속·관련 문서를 포함한다.

프롬프트 파일에는 README 리팩토링 계획 실행 요청부터
Report/Prompting export 요청까지의 대화형 transcript를 정리한다.
```

## Expected Output Files

- `include/ChannelPolicy.h`
- `include/DigitInputBuffer.h`
- `include/TVController.h` (리팩토링 완료)
- `Report/08_tv_controller_refactoring_implementation_report.md`
- `Prompting/08_tv_controller_refactoring_implementation_report-Prompt.md`

## 재사용 프롬프트 (요약)

동일 작업을 재현할 때 사용할 P/C/T/F 블록:

```text
@TVController.h @requirements_analysis.md @README.md

[P] C++ 리팩토링 코드 개발자입니다.
[C] 채널 0~99, 각 Phase 후 ctest 77/77 Green, 동작 변경 없음
[T] README.md 「TVController 모던 C++ 리팩토링 계획」Phase 0~8을 순차 구현해줘
    - Phase별 리팩토링 후 cmake --build && ctest 검증
[F] 리팩토링된 최종 코드 + Report/Prompting export
```

Report·Prompting만 필요할 때:

```text
Report 폴더에 보고서 작성해주고, 현재까지의 프롬프트를 대화형 프롬프트로
Prompting 폴더에 Export transcript 해주는데 파일명은 보고서 파일명에 -Prompt를 붙여서 저장해줘.
```
