# DataMonitor-shp-260715

반도체 시료 생산주문관리 시스템의 **데이터 모니터링 Tool PoC**다. `Json/` PoC가
확정한 JSON 파일(`data/samples.json`, `data/orders.json`)을 읽어, 콘솔에서
"주문량 확인"(상태별 주문 건수)과 "재고량 확인"(시료별 재고 여유/부족/고갈)을
조회하는 **읽기 전용** 도구를 검증한다. 상태 변경(주문 승인/거절, 출고 처리 등)
기능은 이 PoC의 범위가 아니다.

이 저장소는 상위 통합 저장소(`producing/`)의 `Monitor/` 디렉토리를 독립된 Git
Repository로 분리해 제출한 것이다. 다른 PoC 디렉토리(`MVC/`, `Json/`, `Dummy/`,
`Main/`)와는 코드를 직접 공유하지 않으며, 이 저장소 안에 필요한 최소 JSON
리더 로직을 자체적으로 구현한다.

## 디렉토리 구조

```
Monitor/
├── CLAUDE.md                     # 이 PoC의 목표/설계 방향
├── README.md                      # (본 문서)
├── docs/
│   ├── Plan.md                    # 전체 phase 계획, 확정 스키마, 판정 기준 요약
│   ├── phase1-order-aggregation.md
│   ├── phase2-stock-status.md
│   ├── phase3-json-loader.md
│   └── phase4-console-view.md
└── Project1/
    ├── Project1.slnx              # Visual Studio 솔루션 (x64/x86)
    ├── Project1/                  # 콘솔 앱 본체
    │   ├── OrderTypes.h           # Order, OrderStatus, OrderStatusCounts
    │   ├── SampleTypes.h          # Sample
    │   ├── StockTypes.h           # StockLevel, StockStatus
    │   ├── OrderAggregation.h/.cpp    # CountOrdersByStatus (Phase 1)
    │   ├── StockStatusJudge.h/.cpp    # JudgeStockStatus (Phase 2)
    │   ├── JsonReader.h/.cpp          # LoadSamples/LoadOrders (Phase 3)
    │   ├── MonitorAssembly.h/.cpp     # BuildStockLevels (Phase 3)
    │   ├── MonitorSnapshot.h/.cpp     # MonitorSnapshot/RefreshSnapshot (Phase 4)
    │   ├── ConsoleRenderer.h/.cpp     # RenderOrderCounts/RenderStockLevels (Phase 4)
    │   ├── Main.cpp                   # 콘솔 진입점(조립 + 갱신 루프)
    │   └── Project1.vcxproj
    └── ProjectTest/                # gtest 테스트 프로젝트
        ├── OrderAggregationTest.cpp    (8 tests)
        ├── StockStatusJudgeTest.cpp    (9 tests)
        ├── JsonReaderTest.cpp          (8 tests)
        ├── MonitorAssemblyTest.cpp     (4 tests)
        ├── ConsoleRendererTest.cpp     (7 tests)
        ├── MonitorSnapshotTest.cpp     (2 tests)
        ├── fixtures/                  # 테스트용 JSON fixture 파일
        │   ├── orders_valid.json / orders_empty.json
        │   ├── orders_all_statuses.json / orders_unknown_status.json
        │   ├── samples_valid.json / samples_empty.json
        │   └── monitor_snapshot/orders.json, samples.json
        └── ProjectTest.vcxproj
```

총 38개 gtest 테스트(8+9+8+4+7+2)가 모두 통과한다.

## 핵심 기능

### 1. 상태별 주문 수 집계 — `OrderAggregation.h/.cpp`

```cpp
OrderStatusCounts CountOrdersByStatus(const std::vector<Order>& orders);
```

`RESERVED`/`CONFIRMED`/`PRODUCING`/`RELEASE` 건수를 각각 센다. `OrderStatusCounts`
구조체 자체에 `REJECTED` 필드가 없어, "REJECTED는 모니터링 대상이 아니다"라는
정책을 타입으로 표현한다(REJECTED 주문은 집계에서 항상 제외됨). 부작용 없는
순수 함수이며 입력 벡터를 변경하지 않는다.

### 2. 재고 여유/부족/고갈 판정 — `StockStatusJudge.h/.cpp`

```cpp
StockStatus JudgeStockStatus(int stock, int demand);
StockStatus JudgeStockStatus(const StockLevel& level);
```

`stock`(재고)과 `demand`(해당 시료의 미출고 주문 수량 합계: RESERVED+CONFIRMED+
PRODUCING, RELEASE/REJECTED 제외)를 받아 다음 기준으로 판정한다.

| 조건 | 판정 |
|---|---|
| `stock == 0` | **DEPLETED (고갈)** — demand 값과 무관하게 최우선 적용 |
| `stock > 0 && stock < demand` | **SHORTAGE (부족)** |
| `stock > 0 && stock >= demand` (동률 포함, `demand == 0` 포함) | **SUFFICIENT (여유)** |

경계값 정책: `stock == demand`(둘 다 양수)는 재고가 수요를 정확히 충당할 수
있으므로 "부족"이 아니라 "여유"로 판정한다. 이 경계값과 `stock == 0`인 경우는
`StockStatusJudgeTest.cpp`에서 테스트로 고정되어 있다. 음수 입력(도메인상
발생해서는 안 되는 값)은 0으로 clamp하는 방어적 정책을 둔다.

### 3. JSON 로딩 — `JsonReader.h/.cpp`

```cpp
std::vector<Sample> LoadSamples(const std::string& path);
std::vector<Order> LoadOrders(const std::string& path);
```

아래 "JSON 스키마" 절에서 정의한 포맷을 읽는다. `nlohmann::json`으로 파싱한다.
정책:
- 파일이 없으면 `std::runtime_error`.
- `status` 문자열이 알려진 5개 값(`RESERVED`/`REJECTED`/`PRODUCING`/
  `CONFIRMED`/`RELEASE`) 중 하나가 아니면 `std::runtime_error`(무시하지 않고
  실패를 드러냄).
- 목록이 비어 있으면 빈 벡터를 반환.

### 4. 조립 — `MonitorAssembly.h/.cpp`, `MonitorSnapshot.h/.cpp`

```cpp
std::vector<StockLevel> BuildStockLevels(const std::vector<Sample>& samples,
                                          const std::vector<Order>& orders);

struct MonitorSnapshot {
    OrderStatusCounts orderCounts;
    std::vector<StockLevel> stockLevels;
};
MonitorSnapshot RefreshSnapshot(const std::string& dataDir);
```

`BuildStockLevels`는 주문 목록에서 시료별 `demand`(RESERVED+CONFIRMED+PRODUCING
수량 합)를 계산해 각 시료의 `StockLevel{sampleId, stock, demand}`를 만든다.
`RefreshSnapshot`은 `dataDir/samples.json`, `dataDir/orders.json`을 매번 새로
읽어(`LoadSamples`/`LoadOrders`) `CountOrdersByStatus`/`BuildStockLevels`로
조립한 `MonitorSnapshot`을 반환한다 — **이 함수를 호출할 때마다 파일을 다시
읽으므로, 반복 호출 자체가 "갱신(재조회)" 동작**이다.

### 5. 콘솔 렌더링 — `ConsoleRenderer.h/.cpp`

```cpp
std::string RenderOrderCounts(const OrderStatusCounts& counts);
std::string RenderStockLevels(const std::vector<StockLevel>& levels);
```

`std::cout`을 직접 호출하지 않는 순수 문자열 생성 함수로, 집계/판정 결과를
사람이 읽을 수 있는 텍스트로 만든다. 재고 판정 라벨은 "여유"/"부족"/"고갈"로
표시한다. 시료 목록이 비어 있으면 "데이터 없음"을 반환한다. `Main.cpp`는 이
문자열들을 `std::cout`으로 출력하기만 한다(콘솔 I/O와 렌더링 로직 분리).

## JSON 스키마

이 저장소가 읽는 데이터 파일의 스키마는 다음과 같다.

- 파일 경로: `data/samples.json`, `data/orders.json` (기본값. 실행 시 인자로
  디렉토리 변경 가능)
- 최상위 래핑: `{"samples": [...]}`, `{"orders": [...]}`
- 시료(camelCase): `sampleId`, `name`, `avgProductionTime`, `yield`, `stock`
- 주문(camelCase): `orderId`, `sampleId`, `customerName`, `quantity`, `status`,
  `createdAt`
- `status` 문자열: `RESERVED` / `REJECTED` / `PRODUCING` / `CONFIRMED` /
  `RELEASE`

## 사용 라이브러리

- [nlohmann/json](https://github.com/nlohmann/json) (NuGet `nlohmann.json`,
  3.11.3) — JSON 파싱
- [GoogleTest](https://github.com/google/googletest) (NuGet
  `Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn`) — 단위 테스트

## 빌드 방법

Visual Studio에서 `Project1/Project1.slnx`를 열어 빌드하거나, MSBuild로
직접 빌드한다.

```powershell
msbuild Project1/Project1.slnx /p:Configuration=Debug /p:Platform=x64
```

NuGet 패키지 복원이 필요하면 먼저 다음을 실행한다.

```powershell
nuget restore Project1/Project1.slnx
```

## 테스트 실행 방법

`ProjectTest` 프로젝트를 빌드하면 gtest 실행 파일이 생성된다.

```powershell
msbuild Project1/ProjectTest/ProjectTest.vcxproj /p:Configuration=Debug /p:Platform=x64
```

빌드 후 생성된 `ProjectTest.exe`를 실행하면 38개 테스트가 모두 통과하는 것을
확인할 수 있다(Visual Studio Test Explorer에서 실행해도 동일).

## 실행 방법

```powershell
Project1.exe [데이터_디렉토리]
```

- 인자를 생략하면 기본값 `data`(즉 `data/samples.json`, `data/orders.json`)를
  읽는다.
- 실행 시 곧바로 "주문량 확인"과 "재고량 확인" 결과를 콘솔에 한 번 출력한다.
- 이후 프롬프트(`>`)에서 명령을 입력해 조작한다.
  - `r` (또는 빈 입력): 파일을 다시 읽어(`RefreshSnapshot`) 최신 상태로 갱신
    출력한다. 재실행 없이 최신 데이터를 반영한다.
  - `q`: 종료한다.
- 데이터 파일이 없거나 형식이 잘못되면(알 수 없는 `status` 등) 예외 메시지를
  출력하고 계속 명령을 받는다(앱이 죽지 않음).

## TDD 진행 과정 (phase 구성)

`docs/Plan.md`가 정의한 4개 phase를 순서대로 Red → Green으로 진행했다(커밋
이력 기준).

1. **테스트 하네스 구성**: gtest 프로젝트(`ProjectTest`) 세팅 및 상태별 주문 수
   집계 테스트 초안.
2. **Phase 1 (집계)**: `OrderTypes`/`CountOrdersByStatus` 구현. REJECTED 제외를
   포함한 다양한 상태 조합을 테스트로 고정.
3. **Phase 2 (판정)**: `JudgeStockStatus` 구현. `stock == 0`, `stock == demand`
   등 경계값을 테스트로 고정.
4. **Phase 3 (로딩/조립)**: `JsonReader`(Json/ 스키마 파싱), `BuildStockLevels`
   (시료별 demand 조립)를 fixture JSON 파일 기반으로 테스트.
5. **Phase 4 (렌더링/갱신)**: `ConsoleRenderer`(순수 문자열 렌더링),
   `MonitorSnapshot`/`RefreshSnapshot`(갱신 스냅샷 조립)를 테스트하고, 마지막으로
   `Main.cpp`에서 콘솔 조회/갱신 루프를 연결.

각 phase는 "집계: 상태별 주문 수 함수/테스트", "판정: 재고 여유/부족/고갈
함수/테스트"와 같은 작은 단위 커밋으로 기록되어 있다(`git log` 참고).

## 범위 밖 (이 PoC가 하지 않는 것)

- **읽기 전용 도구다.** 주문 승인/거절, 출고 처리 등 상태를 변경하는 로직은
  구현하지 않는다.
- 시료/주문 데이터의 생성·수정은 `Json/`(영속성 PoC), `Dummy/`(더미 데이터
  생성 PoC)의 책임이며, 이 저장소는 그 결과 파일을 읽기만 한다.
- 이 PoC에서 검증한 집계/판정 로직(`CountOrdersByStatus`, `JudgeStockStatus`,
  `BuildStockLevels` 등)은 사람이 읽고 다시 구현하는 방식으로 `Main/`(최종
  통합 시스템)의 "모니터링" 메뉴에 반영된다. 코드가 직접 import되지는 않는다.
