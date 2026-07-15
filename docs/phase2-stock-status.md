# Phase 2: 재고 상태 판정 (여유/부족/고갈)

[← Plan.md로 돌아가기](./Plan.md)

## 목표

시료 하나의 현재 재고와, 해당 시료에 대한 미출고 주문 수량 합계(수요)를 입력받아
"여유(SUFFICIENT) / 부족(SHORTAGE) / 고갈(DEPLETED)"을 판정하는 순수 함수를
TDD로 구현한다. 경계값(정확히 0, 재고와 수요가 정확히 같을 때)을 반드시 테스트로
고정한다.

이 phase도 Phase 1과 마찬가지로 파일 I/O와 무관한 순수 로직이므로 Json/ 스키마
확정과 무관하게 진행 가능하다.

## 판정 기준 (고정)

시료의 현재 재고를 `stock`, 해당 시료에 대해 아직 출고되지 않은 주문(RESERVED +
CONFIRMED + PRODUCING, RELEASE/REJECTED 제외) 수량의 합을 `demand`라 할 때:

| 조건 | 판정 |
|---|---|
| `stock == 0` | 고갈 (DEPLETED) |
| `stock > 0` and `stock < demand` | 부족 (SHORTAGE) |
| `stock > 0` and `stock >= demand` (동률 포함) | 여유 (SUFFICIENT) |
| `demand == 0` (주문이 없음) and `stock > 0` | 여유 (SUFFICIENT) |
| `demand == 0` and `stock == 0` | 고갈 (DEPLETED) — 재고 0 규칙이 우선 |

`stock == 0`이 다른 모든 조건보다 우선(고갈)한다는 점에 주의: 수요가 0이어도
현재 재고가 0이면 "고갈"로 표시한다 (판매 가능한 재고가 전혀 없는 상태이므로).

## 도메인 타입

```cpp
enum class StockStatus { SUFFICIENT, SHORTAGE, DEPLETED };

struct StockLevel {
    std::string sampleId;
    int stock;
    int demand; // 미출고 주문 수량 합계
};
```

## 작성할 테스트 목록 (Red 단계)

1. `stock == 0, demand == 0` → DEPLETED (재고 0 최우선 규칙).
2. `stock == 0, demand > 0` (예: stock=0, demand=10) → DEPLETED.
3. `stock > 0, demand == 0` (예: stock=5, demand=0) → SUFFICIENT (주문이 없으면
   여유).
4. `stock > 0, stock < demand` (예: stock=3, demand=10) → SHORTAGE.
5. **경계값: `stock == demand`** (예: stock=10, demand=10, 둘 다 0이 아님) →
   SUFFICIENT (동률은 "부족"이 아니라 "여유"로 판정 — 핵심 경계 테스트).
6. `stock > demand` (예: stock=10, demand=3) → SUFFICIENT.
7. **음수 입력 방어**: 시료 목록에 없는 sampleId의 주문(고아 주문)이나 음수
   quantity가 조립 단계에서 넘어올 가능성에 대비해, `demand < 0`이거나
   `stock < 0`인 경우의 동작을 명시적으로 결정하고 테스트로 고정한다 (예:
   도메인상 발생하지 않아야 하는 값이므로 방어적으로 assert 또는 0으로
   clamp — 구현 시 택일하여 테스트에 반영).

## 구현할 함수/파일

- `Monitor/Project1/Project1/StockTypes.h` — `StockStatus`, `StockLevel` 정의.
- `Monitor/Project1/Project1/StockStatusJudge.h` /
  `StockStatusJudge.cpp` — `StockStatus JudgeStockStatus(int stock, int
  demand);` (순수 함수, `StockLevel`을 감싸는 오버로드도 제공 가능).
- `Monitor/Project1/ProjectTest/StockStatusJudgeTest.cpp` — 위 7개 케이스를
  gtest로 작성 (파라미터화 테스트 권장: `{stock, demand, expected}` 튜플).

## 완료 기준

- 위 7개 테스트가 모두 gtest에서 통과.
- `stock == demand` 동률 케이스가 SUFFICIENT로 고정되어 있고, 이 판단 근거가
  주석 또는 테스트 이름에 명시되어 있음 (구현 세부사항이 아니라 의도적 정책임을
  기록).
- `JudgeStockStatus`는 부작용이 없고 입력만으로 결과가 결정되는 순수 함수임.

## 다음 phase와의 연결점

- Phase 3은 파싱된 `Sample` 리스트와 Phase 1에서 이미 로딩된 `Order` 리스트로부터
  시료별 `demand`(미출고 주문 수량 합계)를 계산해 `StockLevel`을 구성하고,
  `JudgeStockStatus`에 전달한다. `demand` 계산 로직 자체(시료별 그룹핑 및 합산)는
  Phase 3의 책임이며, 이 문서의 판정 함수는 이미 계산된 `stock`/`demand` 숫자만
  받는다.
- Phase 4는 `StockStatus` 값을 사람이 읽을 수 있는 한글 라벨("여유"/"부족"/
  "고갈")로 변환해 콘솔에 출력하는 책임만 가진다.
