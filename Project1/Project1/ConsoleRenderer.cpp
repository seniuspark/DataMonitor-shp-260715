#include "ConsoleRenderer.h"

#include <sstream>

#include "StockStatusJudge.h"

namespace monitor {

namespace {

std::string StockStatusLabel(StockStatus status) {
    switch (status) {
        case StockStatus::SUFFICIENT:
            return "여유";
        case StockStatus::SHORTAGE:
            return "부족";
        case StockStatus::DEPLETED:
            return "고갈";
    }
    return "알수없음";
}

}  // namespace

std::string RenderOrderCounts(const OrderStatusCounts& counts) {
    std::ostringstream out;
    out << "RESERVED: " << counts.reserved << "\n";
    out << "CONFIRMED: " << counts.confirmed << "\n";
    out << "PRODUCING: " << counts.producing << "\n";
    out << "RELEASE: " << counts.release << "\n";
    return out.str();
}

std::string RenderStockLevels(const std::vector<StockLevel>& levels) {
    if (levels.empty()) {
        return "데이터 없음\n";
    }

    std::ostringstream out;
    for (const StockLevel& level : levels) {
        out << level.sampleId << " stock=" << level.stock << " demand=" << level.demand
            << " [" << StockStatusLabel(JudgeStockStatus(level)) << "]\n";
    }
    return out.str();
}

}  // namespace monitor
