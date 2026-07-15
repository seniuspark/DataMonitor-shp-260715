#pragma once

#include <string>

namespace monitor {

enum class OrderStatus {
    RESERVED,
    REJECTED,
    PRODUCING,
    CONFIRMED,
    RELEASE
};

struct Order {
    std::string orderId;
    std::string sampleId;
    std::string customerName;
    int quantity;
    OrderStatus status;
    std::string createdAt;
};

struct OrderStatusCounts {
    int reserved = 0;
    int confirmed = 0;
    int producing = 0;
    int release = 0;
    // REJECTED는 필드 자체가 없음 - 모니터링 집계 대상이 아님을 타입으로 표현
};

}  // namespace monitor
