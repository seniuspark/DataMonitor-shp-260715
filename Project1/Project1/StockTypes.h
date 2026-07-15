#pragma once

#include <string>

namespace monitor {

enum class StockStatus {
    SUFFICIENT,
    SHORTAGE,
    DEPLETED
};

struct StockLevel {
    std::string sampleId;
    int stock;
    int demand;  // 미출고(RESERVED+CONFIRMED+PRODUCING) 주문 수량 합계
};

}  // namespace monitor
