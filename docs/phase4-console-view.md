# Phase 4: 콘솔 렌더링 & 갱신 동작

[← Plan.md로 돌아가기](./Plan.md)

## 목표

Phase 1~3에서 검증한 순수 함수/조립 로직을 이용해, 실제 콘솔 화면에 "상태별
주문 수"와 "시료별 재고 상태(여유/부족/고갈)"를 출력하고, 재실행 없이 최신
데이터를 다시 조회할 수 있는 갱신 동작(명령 입력 또는 폴링)을 제공한다.

렌더링과 갱신 루프는 판정/집계 로직과 반드시 분리되어, 콘솔 I/O를 흉내낸
스텁(`std::ostream&`을 주입하는 등)으로 출력 문자열을 gtest로 검증할 수 있게
설계한다.

## 작성할 테스트 목록 (Red 단계)

렌더링 함수 자체는 문자열 생성 로직이므로 순수 함수에 가깝게 만들고, 이 부분만
gtest로 검증한다 (실제 콘솔 루프/입력 대기는 수동 실행으로 확인).

1. **상태별 집계 렌더링**: `OrderStatusCounts{reserved=2, confirmed=1,
   producing=3, release=1}`을 넘기면, 각 상태명과 숫자가 포함된 문자열(또는
   줄 단위 리스트)이 생성됨. REJECTED 관련 텍스트가 전혀 포함되지 않음.
2. **재고 상태 렌더링 — 여유**: `StockLevel{sampleId="S-001", stock=10,
   demand=3}` → 출력에 "여유"라는 한글 라벨과 시료ID, 재고, 수요 값이 포함됨.
3. **재고 상태 렌더링 — 부족**: `stock=3, demand=10` → "부족" 라벨 포함.
4. **재고 상태 렌더링 — 고갈**: `stock=0, demand=5` → "고갈" 라벨 포함.
5. **여러 시료 목록 렌더링**: 시료 3개(여유/부족/고갈 각 1개씩)를 넘기면 3줄이
   출력되고 각 줄의 라벨이 대응하는 판정과 정확히 일치함.
6. **빈 스냅샷 렌더링**: 주문/시료가 모두 0건일 때 크래시 없이 "데이터 없음" 류
   안내 문구를 출력.

## 구현할 함수/파일

- `Monitor/Project1/Project1/ConsoleRenderer.h` / `ConsoleRenderer.cpp` —
  `std::string RenderOrderCounts(const OrderStatusCounts&);`,
  `std::string RenderStockLevels(const std::vector<StockLevel>&);` (순수
  문자열 생성 함수, `std::cout` 직접 호출 없음).
- `Monitor/Project1/Project1/MonitorSnapshot.h` — Phase 3의 로딩 결과를 담는
  `struct MonitorSnapshot { OrderStatusCounts orderCounts; std::vector<StockLevel>
  stockLevels; };` 및 이를 만드는 `MonitorSnapshot RefreshSnapshot(const
  std::string& dataPath);` (Phase 3 함수들을 조합).
- `Monitor/Project1/Project1/main.cpp` — 메뉴 루프: 최초 조회 출력 후,
  사용자가 갱신 명령(예: `r` 입력 또는 Enter)을 입력할 때마다
  `RefreshSnapshot`을 다시 호출해 화면을 다시 그림. 종료 명령(`q` 등) 제공.
  이 파일은 테스트 대상이 아니라 조립/실행 진입점이며, 로직은 전부 위 함수들에
  위임한다.
- `Monitor/Project1/ProjectTest/ConsoleRendererTest.cpp` — 위 6개 테스트.

## 완료 기준

- 위 6개 렌더링 테스트가 모두 gtest에서 통과.
- `main.cpp`를 빌드/실행해 콘솔에 상태별 주문 수와 시료별 재고 상태가 정상
  출력되는 것을 최소 1회 수동 실행으로 확인한다 (fixture 또는 실제 Json/
  산출물 파일 사용).
- 데이터 파일을 수동으로 변경한 뒤 갱신 명령을 입력하면, 앱을 재시작하지 않고도
  변경된 값이 화면에 반영됨을 수동으로 확인한다.
- `ConsoleRenderer`의 함수들은 `std::cout`을 직접 호출하지 않고 문자열만
  반환하여, 렌더링 내용과 실제 출력(콘솔에 쓰는 행위)이 분리되어 있다.

## 다음 phase와의 연결점

- 이 phase가 Monitor PoC의 마지막 phase다. 여기서 검증된 집계/판정/렌더링
  설계는 `Main/`의 "모니터링" 메뉴에 사람이 읽고 다시 구현(또는 파일 복사)하는
  방식으로 통합된다 (코드 직접 import는 하지 않음 — 상위 `../CLAUDE.md` 참고).
