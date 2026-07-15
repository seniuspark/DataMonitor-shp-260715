#pragma once

#include <vector>

#include "OrderTypes.h"
#include "SampleTypes.h"
#include "StockTypes.h"

namespace monitor {

// 시료 목록과 주문 목록으로부터 시료별 StockLevel(stock, demand)을 조립한다.
// demand는 RELEASE/REJECTED를 제외한(RESERVED+CONFIRMED+PRODUCING) 주문
// 수량의 합이다. 부작용 없는 순수 함수다.
std::vector<StockLevel> BuildStockLevels(const std::vector<Sample>& samples,
                                          const std::vector<Order>& orders);

}  // namespace monitor
