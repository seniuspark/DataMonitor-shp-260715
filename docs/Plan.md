# Monitor PoC — 전체 개발 계획

이 문서는 `Monitor/CLAUDE.md`(목표/설계 방향/TDD 진행 방식)와 상위 `../CLAUDE.md`
(공통 도메인 모델)을 근거로, `Monitor/` PoC를 여러 phase로 나눈 전체 설계를
정리한다.

## 전제 조건 (중요)

- **Json/ 스키마 확정됨**: 이 PoC는 `Json/` PoC가 정한 저장 포맷(파일 경로,
  JSON 필드명, 주문/시료 구조)을 그대로 읽어야 한다. `Json/docs/Plan.md` 기준
  실제 JSON 스키마가 아래와 같이 확정되었다.
- Phase 1~2(순수 함수)는 스키마와 무관한 내부 도메인 구조체(`Order`, `Sample`)만
  다루므로 아래 확정 스키마와 무관하게 진행할 수 있다. 단, 내부 C++ 구조체의
  멤버 네이밍은 C++ 컨벤션(camelCase 등)을 유지해도 무방하며, 중요한 것은 JSON
  파일을 읽는 Phase 3에서 파싱 대상 키(key)가 아래 확정 스키마와 정확히
  일치해야 한다는 점이다.
- 확정 스키마 (`Json/docs/Plan.md` 기준):
  - 파일 경로: `data/samples.json`, `data/orders.json`
  - 최상위 래핑: `{"samples": [...]}`, `{"orders": [...]}`
  - 시료(camelCase 필드): `sampleId`, `name`, `avgProductionTime`, `yield`,
    `stock`
  - 주문(camelCase 필드): `orderId`, `sampleId`, `customerName`, `quantity`,
    `status`, `createdAt`
  - `status`는 문자열: `RESERVED`/`REJECTED`/`PRODUCING`/`CONFIRMED`/`RELEASE`

## Phase 구성 개요

| Phase | 이름 | 목표 | 의존 관계 |
|---|---|---|---|
| 1 | 상태별 주문 수 집계 | 순수 함수로 상태별(RESERVED/CONFIRMED/PRODUCING/RELEASE) 주문 수 집계, REJECTED 제외 | 없음 (독립 시작) |
| 2 | 재고 상태 판정 | 순수 함수로 시료별 재고 여유/부족/고갈 판정, 경계값 테스트 | 없음 (Phase 1과 병렬 가능) |
| 3 | JSON 로딩 & 조립 | Json/ 스키마를 읽어 Order/Sample 구조체로 파싱하고 Phase 1/2 함수에 전달하는 조립 로직 | Phase 1, 2 완료 + Json/ 스키마 확정 확인 |
| 4 | 콘솔 렌더링 & 갱신 | 집계/판정 결과를 콘솔에 표 형태로 출력, 갱신(재조회) 명령/폴링 제공 | Phase 3 완료 |

Phase 1과 2는 서로 의존하지 않는 순수 함수 작업이므로 순서를 바꾸거나 병렬로
진행해도 무방하다. Phase 3은 반드시 1, 2가 끝난 뒤 진행하며, 착수 직전
Json/ 스키마 재확인이 선행 조건이다. Phase 4는 Phase 3 산출물(로딩된 데이터 +
조립 로직)에 의존한다.

## 세부 계획 문서 링크

- [Phase 1: 상태별 주문 수 집계](./phase1-order-aggregation.md)
- [Phase 2: 재고 상태 판정 (여유/부족/고갈)](./phase2-stock-status.md)
- [Phase 3: JSON 로딩 & 조립 로직](./phase3-json-loader.md)
- [Phase 4: 콘솔 렌더링 & 갱신 동작](./phase4-console-view.md)

## 판정 기준 요약 (전체 phase 공통 고정값)

- **주문 상태 집계 대상**: `RESERVED`, `CONFIRMED`, `PRODUCING`, `RELEASE` 만
  집계. `REJECTED`는 항상 제외 (도메인 모델 상 "모니터링 대상 아님").
- **재고 판정 기준** (시료 하나 기준, 재고 수량 `stock`, 해당 시료의 미출고
  주문 수량 합계 `demand` — RELEASE/REJECTED를 제외한 RESERVED/CONFIRMED/
  PRODUCING 주문 수량의 합):
  - `stock == 0` → **고갈**
  - `stock > 0 && stock < demand` → **부족**
  - 그 외 (`stock >= demand`, `demand == 0` 포함) → **여유**
  - 정확한 동률(`stock == demand`, `demand > 0`)은 **여유**로 판정 (재고가
    수요를 정확히 충당할 수 있으므로 "부족"이 아님). 이 경계값은 Phase 2에서
    테스트로 고정한다.

## 완료 기준 (PoC 전체)

- Phase 1, 2의 순수 함수가 각각 gtest로 검증되고 경계값(REJECTED 제외 여부,
  `stock == 0`, `stock == demand`)이 테스트로 고정되어 있다.
- Phase 3에서 실제(또는 fixture) JSON 파일을 읽어 Phase 1/2 함수에 데이터를
  넘기는 조립 로직이 테스트로 검증된다.
- Phase 4에서 콘솔에 상태별 주문 수와 시료별 재고 상태가 출력되고, 재실행 없이
  갱신할 수 있는 명령(또는 폴링)이 동작한다. 이 계층은 수동 실행으로 확인한다.
- 각 phase는 작은 단위 커밋으로 기록된다 (예: "집계: 상태별 주문 수 함수/테스트",
  "판정: 재고 여유/부족/고갈 함수/테스트").
