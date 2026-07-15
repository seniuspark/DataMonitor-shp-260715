#pragma once

#include <vector>

#include "OrderTypes.h"

namespace monitor {

// 주문 목록에서 상태별(RESERVED/CONFIRMED/PRODUCING/RELEASE) 건수를 집계한다.
// REJECTED 상태 주문은 결과에 포함되지 않는다 (모니터링 대상 아님).
// 입력 벡터를 변경하지 않는 순수 함수다.
OrderStatusCounts CountOrdersByStatus(const std::vector<Order>& orders);

}  // namespace monitor
