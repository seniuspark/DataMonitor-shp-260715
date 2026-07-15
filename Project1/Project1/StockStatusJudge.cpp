#include "StockStatusJudge.h"

#include <algorithm>

namespace monitor {

StockStatus JudgeStockStatus(int stock, int demand) {
    // 도메인상 발생해서는 안 되는 음수 입력에 대한 방어적 clamp 정책.
    int clampedStock = std::max(stock, 0);
    int clampedDemand = std::max(demand, 0);

    if (clampedStock == 0) {
        return StockStatus::DEPLETED;
    }
    if (clampedStock < clampedDemand) {
        return StockStatus::SHORTAGE;
    }
    return StockStatus::SUFFICIENT;
}

StockStatus JudgeStockStatus(const StockLevel& level) {
    return JudgeStockStatus(level.stock, level.demand);
}

}  // namespace monitor
