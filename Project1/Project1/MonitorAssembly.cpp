#include "MonitorAssembly.h"

#include <unordered_map>

namespace monitor {

namespace {

bool CountsTowardDemand(OrderStatus status) {
    switch (status) {
        case OrderStatus::RESERVED:
        case OrderStatus::CONFIRMED:
        case OrderStatus::PRODUCING:
            return true;
        case OrderStatus::RELEASE:
        case OrderStatus::REJECTED:
            return false;
    }
    return false;
}

}  // namespace

std::vector<StockLevel> BuildStockLevels(const std::vector<Sample>& samples,
                                          const std::vector<Order>& orders) {
    std::unordered_map<std::string, int> demandBySampleId;
    for (const Order& order : orders) {
        if (CountsTowardDemand(order.status)) {
            demandBySampleId[order.sampleId] += order.quantity;
        }
    }

    std::vector<StockLevel> levels;
    levels.reserve(samples.size());
    for (const Sample& sample : samples) {
        int demand = 0;
        auto it = demandBySampleId.find(sample.sampleId);
        if (it != demandBySampleId.end()) {
            demand = it->second;
        }
        levels.push_back(StockLevel{sample.sampleId, sample.stock, demand});
    }
    return levels;
}

}  // namespace monitor
