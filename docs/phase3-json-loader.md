# Phase 3: JSON 로딩 & 조립 로직

[← Plan.md로 돌아가기](./Plan.md)

## 착수 전 필수 확인

**이 phase를 시작하기 전에 반드시 `Json/docs/Plan.md`(및 확정된 구현)를 다시
읽고, 실제 파일 경로/JSON 필드명/타입이 아래 "가정 스키마"와 일치하는지
확인한다.** 다르다면 이 문서와 `OrderTypes.h`/`SampleTypes.h`를 실제 스키마에
맞게 먼저 갱신한 뒤 진행한다. `Json/` 코드는 직접 import할 수 없으므로 이 파일
안에 최소 리더 로직을 다시 구현한다 (읽기 전용, 쓰기 로직 없음).

## 목표

Json/ 저장 포맷과 동일한 JSON 파일(주문 목록, 시료 목록)을 읽어 `Order`/`Sample`
구조체 리스트로 파싱하고, Phase 1의 `CountOrdersByStatus`와 Phase 2의
`JudgeStockStatus`에 필요한 입력(`StockLevel` 목록 등)을 조립하는 로직을
TDD로 구현한다.

## 가정 스키마 (Json/ 확정 전 잠정, 재확인 필수)

```json
{
  "samples": [
    { "sampleId": "S-001", "name": "...", "avgProductionTime": 30, "yield": 0.9, "stock": 12 }
  ],
  "orders": [
    { "orderId": "ORD-20260416-0043", "sampleId": "S-001", "customerName": "...",
      "quantity": 5, "status": "RESERVED", "createdAt": "2026-04-16T10:00:00" }
  ]
}
```

## 도메인 타입 (Phase 1/2 타입 재사용)

```cpp
struct Sample {
    std::string sampleId;
    std::string name;
    int avgProductionTime;
    double yield;
    int stock;
};
```

`Order`, `OrderStatus`, `StockLevel`은 Phase 1/2에서 정의한 타입을 그대로 사용.

## 작성할 테스트 목록 (Red 단계)

1. **정상 파일 파싱**: 시료 2개 + 주문 3개(RESERVED/CONFIRMED/REJECTED 혼합)를
   담은 fixture JSON 파일을 읽어 `std::vector<Sample>`, `std::vector<Order>`
   개수와 필드값이 정확히 매칭되는지 검증.
2. **상태 문자열 → enum 변환**: `"RESERVED"`, `"REJECTED"`, `"PRODUCING"`,
   `"CONFIRMED"`, `"RELEASE"` 각 문자열이 올바른 `OrderStatus` 값으로 매핑됨.
3. **알 수 없는 상태 문자열**: 스키마에 없는 값(예: `"UNKNOWN"`)이 들어오면 예외를
   던지거나 로딩을 실패시킴 (정책을 정하고 테스트로 고정 — 무시하고 건너뛰는
   대신 실패를 명시적으로 드러내는 편을 기본으로 한다).
4. **파일이 없을 때**: 존재하지 않는 경로를 넘기면 명확한 에러(예외 또는
   `std::optional`/`std::expected` 반환)를 내고, 크래시하지 않음.
5. **빈 파일(주문/시료 모두 0건)**: 빈 리스트를 반환하고 이후 집계 함수에
   넘겼을 때 Phase 1/2 테스트에서 이미 검증한 "빈 목록 → 카운트 0" 케이스와
   일관됨을 통합 테스트로 재확인.
6. **조립 로직: 시료별 demand 계산**: 시료 S-001에 대해 RESERVED 2개(수량
   3, 4)와 CONFIRMED 1개(수량 5)와 RELEASE 1개(수량 100)가 있을 때, demand는
   RELEASE를 제외한 3+4+5=12로 계산됨 (RELEASE/REJECTED 제외 규칙의 통합
   테스트).
7. **조립 로직: 주문에 없는 시료**: 시료 목록에는 있으나 주문이 전혀 없는
   시료는 `demand == 0`으로 `StockLevel`이 만들어짐 (Phase 2의 "demand==0,
   stock>0 → 여유" 케이스와 연결).

## 구현할 함수/파일

- `Monitor/Project1/Project1/JsonReader.h` / `JsonReader.cpp` — 최소 JSON
  파서 또는 기존 라이브러리(nlohmann/json 등, Json/과 동일 방식이라면 동일
  라이브러리) 사용. `LoadSamples(path)`, `LoadOrders(path)` 또는 통합
  `LoadMonitorData(path)` 함수 제공.
- `Monitor/Project1/Project1/MonitorAssembly.h` / `MonitorAssembly.cpp` —
  `std::vector<StockLevel> BuildStockLevels(const std::vector<Sample>&,
  const std::vector<Order>&);` (시료별 demand 합산 후 Phase 2 타입 조립).
- `Monitor/Project1/ProjectTest/JsonReaderTest.cpp`,
  `MonitorAssemblyTest.cpp` — 위 테스트 목록.
- `Monitor/Project1/ProjectTest/fixtures/` — 테스트용 JSON fixture 파일들.

## 완료 기준

- 위 7개 테스트가 모두 gtest에서 통과.
- 로딩 로직은 데이터를 변경하지 않음(읽기 전용) — 파일에 쓰기 연산이 전혀 없음을
  코드 리뷰로 확인.
- `BuildStockLevels`가 Phase 2의 `JudgeStockStatus`와 조합했을 때 올바른
  판정을 내리는 것을 통합 테스트로 1개 이상 검증 (fixture → 로딩 → 조립 →
  판정까지 end-to-end).

## 다음 phase와의 연결점

- Phase 4는 `LoadMonitorData` (또는 `LoadSamples`+`LoadOrders`)와
  `BuildStockLevels`, `CountOrdersByStatus`, `JudgeStockStatus`를 조합해 한 번의
  "조회" 동작을 구성하고, 이를 갱신 명령이 호출할 진입점 함수로 노출한다
  (예: `MonitorSnapshot RefreshSnapshot(const std::string& dataPath);`).
