#include "OrderAggregation.h"

namespace monitor {

OrderStatusCounts CountOrdersByStatus(const std::vector<Order>& orders) {
    OrderStatusCounts counts;

    for (const Order& order : orders) {
        switch (order.status) {
            case OrderStatus::RESERVED:
                ++counts.reserved;
                break;
            case OrderStatus::CONFIRMED:
                ++counts.confirmed;
                break;
            case OrderStatus::PRODUCING:
                ++counts.producing;
                break;
            case OrderStatus::RELEASE:
                ++counts.release;
                break;
            case OrderStatus::REJECTED:
                break;
        }
    }

    return counts;
}

}  // namespace monitor
