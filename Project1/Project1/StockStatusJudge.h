#pragma once

#include "StockTypes.h"

namespace monitor {

// 재고(stock)와 미출고 주문 수량 합계(demand)를 받아 재고 상태를 판정한다.
// 판정 기준 (Monitor/docs/phase2-stock-status.md 고정):
//   - stock == 0                    -> DEPLETED (재고 0 최우선, demand 무관)
//   - stock > 0 && stock < demand   -> SHORTAGE
//   - stock > 0 && stock >= demand  -> SUFFICIENT (동률 포함, demand == 0 포함)
// 부작용 없는 순수 함수다.
StockStatus JudgeStockStatus(int stock, int demand);

StockStatus JudgeStockStatus(const StockLevel& level);

}  // namespace monitor
