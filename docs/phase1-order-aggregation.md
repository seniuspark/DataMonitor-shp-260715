# Phase 1: 상태별 주문 수 집계

[← Plan.md로 돌아가기](./Plan.md)

## 목표

주문 목록(메모리 상 `Order` 구조체 리스트)을 입력받아, 상태별
(`RESERVED`/`CONFIRMED`/`PRODUCING`/`RELEASE`) 주문 건수를 집계하는 순수 함수를
TDD로 구현한다. `REJECTED` 상태 주문은 어떤 경우에도 집계 결과에 포함되지 않아야
한다.

이 phase는 파일 I/O나 콘솔 출력에 의존하지 않는 순수 로직만 다루므로, Json/
스키마 확정 여부와 무관하게 즉시 시작할 수 있다.

## 도메인 타입 (이 phase에서 정의)

```cpp
enum class OrderStatus { RESERVED, REJECTED, PRODUCING, CONFIRMED, RELEASE };

struct Order {
    std::string orderId;
    std::string sampleId;
    std::string customerName;
    int quantity;
    OrderStatus status;
    std::string createdAt;
};

struct OrderStatusCounts {
    int reserved = 0;
    int confirmed = 0;
    int producing = 0;
    int release = 0;
    // REJECTED는 필드 자체가 없음 — 집계 대상이 아님을 타입으로 표현
};
```

## 작성할 테스트 목록 (Red 단계)

1. **빈 목록** → 모든 카운트가 0.
2. **단일 상태 각각**: RESERVED 1건만 있을 때 `reserved == 1`, 나머지 0. 동일하게
   CONFIRMED, PRODUCING, RELEASE 각각에 대해 반복 (4개 테스트 또는
   파라미터화 테스트).
3. **REJECTED만 있는 목록** → 모든 카운트가 0 (REJECTED가 어떤 필드에도
   집계되지 않음을 확인하는 핵심 경계 테스트).
4. **혼합 목록**: RESERVED 2건, CONFIRMED 1건, PRODUCING 3건, RELEASE 1건,
   REJECTED 5건이 섞인 목록 → 각 카운트가 정확히 2/1/3/1이고 REJECTED 5건은
   무시됨.
5. **동일 시료의 여러 주문**: 같은 `sampleId`를 가진 주문 여러 건이 있어도
   `sampleId`와 무관하게 상태별로만 집계됨 (Phase 1은 시료 구분 없이 전체
   집계이므로, sampleId가 결과에 영향 없음을 확인).

## 구현할 함수/파일

- `Monitor/Project1/Project1/OrderTypes.h` — `OrderStatus`, `Order`,
  `OrderStatusCounts` 정의.
- `Monitor/Project1/Project1/OrderAggregation.h` /
  `OrderAggregation.cpp` — `OrderStatusCounts CountOrdersByStatus(const
  std::vector<Order>& orders);`
- `Monitor/Project1/ProjectTest/OrderAggregationTest.cpp` (또는 팀 컨벤션에 맞는
  테스트 프로젝트 경로) — 위 테스트 목록을 gtest `TEST`/`TEST_P`로 작성.

## 완료 기준

- 위 5개 테스트 케이스가 모두 gtest에서 통과.
- `CountOrdersByStatus` 함수는 콘솔 출력이나 파일 접근을 하지 않는 순수 함수임
  (부작용 없음, 입력 벡터를 변경하지 않음 — `const&` 파라미터로 강제).
- REJECTED가 `OrderStatusCounts`에 필드로 존재하지 않아, 구현 실수로 REJECTED를
  세는 것 자체가 컴파일 타임에 어렵게 설계되어 있음.

## 다음 phase와의 연결점

- Phase 3(JSON 로딩)에서 파싱된 `Order` 리스트를 그대로 `CountOrdersByStatus`에
  전달한다. 따라서 `Order` 구조체의 필드명/타입은 Phase 3에서 Json/ 실제
  스키마에 맞춰 조정될 수 있으며, 이 경우 Phase 1 테스트도 함께 갱신한다.
- Phase 4(콘솔 렌더링)는 `OrderStatusCounts`를 입력받아 화면에 표 형태로 출력하는
  책임만 가지며, 집계 로직 자체는 재구현하지 않는다.
