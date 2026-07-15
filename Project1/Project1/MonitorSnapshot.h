#pragma once

#include <string>
#include <vector>

#include "OrderTypes.h"
#include "StockTypes.h"

namespace monitor {

// 한 번의 "조회" 결과를 담는 스냅샷.
struct MonitorSnapshot {
    OrderStatusCounts orderCounts;
    std::vector<StockLevel> stockLevels;
};

// dataDir 아래의 samples.json, orders.json을 매번 새로 읽어(LoadSamples/
// LoadOrders) CountOrdersByStatus/BuildStockLevels로 조립한 MonitorSnapshot을
// 반환한다. 호출할 때마다 파일을 다시 읽으므로, 이 함수를 반복 호출하는 것이
// 곧 "갱신(재조회)" 동작이다.
MonitorSnapshot RefreshSnapshot(const std::string& dataDir);

}  // namespace monitor
