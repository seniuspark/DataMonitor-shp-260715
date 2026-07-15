#include "pch.h"
#include "../Project1/OrderAggregation.h"

namespace {

using monitor::Order;
using monitor::OrderStatus;
using monitor::OrderStatusCounts;
using monitor::CountOrdersByStatus;

TEST(CountOrdersByStatusTest, EmptyListYieldsAllZero) {
    std::vector<Order> orders;

    OrderStatusCounts counts = CountOrdersByStatus(orders);

    EXPECT_EQ(counts.reserved, 0);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);
}

TEST(CountOrdersByStatusTest, SingleReservedOrderCountsOnlyReserved) {
    std::vector<Order> orders{
        Order{"ORD-1", "S-001", "Customer", 10, OrderStatus::RESERVED, "2026-07-15"},
    };

    OrderStatusCounts counts = CountOrdersByStatus(orders);

    EXPECT_EQ(counts.reserved, 1);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);
}

TEST(CountOrdersByStatusTest, SingleConfirmedOrderCountsOnlyConfirmed) {
    std::vector<Order> orders{
        Order{"ORD-1", "S-001", "Customer", 10, OrderStatus::CONFIRMED, "2026-07-15"},
    };

    OrderStatusCounts counts = CountOrdersByStatus(orders);

    EXPECT_EQ(counts.reserved, 0);
    EXPECT_EQ(counts.confirmed, 1);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);
}

TEST(CountOrdersByStatusTest, SingleProducingOrderCountsOnlyProducing) {
    std::vector<Order> orders{
        Order{"ORD-1", "S-001", "Customer", 10, OrderStatus::PRODUCING, "2026-07-15"},
    };

    OrderStatusCounts counts = CountOrdersByStatus(orders);

    EXPECT_EQ(counts.reserved, 0);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 1);
    EXPECT_EQ(counts.release, 0);
}

TEST(CountOrdersByStatusTest, SingleReleaseOrderCountsOnlyRelease) {
    std::vector<Order> orders{
        Order{"ORD-1", "S-001", "Customer", 10, OrderStatus::RELEASE, "2026-07-15"},
    };

    OrderStatusCounts counts = CountOrdersByStatus(orders);

    EXPECT_EQ(counts.reserved, 0);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 1);
}

TEST(CountOrdersByStatusTest, RejectedOnlyListYieldsAllZero) {
    std::vector<Order> orders{
        Order{"ORD-1", "S-001", "Customer", 10, OrderStatus::REJECTED, "2026-07-15"},
        Order{"ORD-2", "S-002", "Customer", 5, OrderStatus::REJECTED, "2026-07-15"},
    };

    OrderStatusCounts counts = CountOrdersByStatus(orders);

    EXPECT_EQ(counts.reserved, 0);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);
}

TEST(CountOrdersByStatusTest, MixedListCountsCorrectlyAndIgnoresRejected) {
    std::vector<Order> orders{
        Order{"ORD-1", "S-001", "Customer", 10, OrderStatus::RESERVED, "2026-07-15"},
        Order{"ORD-2", "S-001", "Customer", 10, OrderStatus::RESERVED, "2026-07-15"},
        Order{"ORD-3", "S-001", "Customer", 10, OrderStatus::CONFIRMED, "2026-07-15"},
        Order{"ORD-4", "S-001", "Customer", 10, OrderStatus::PRODUCING, "2026-07-15"},
        Order{"ORD-5", "S-001", "Customer", 10, OrderStatus::PRODUCING, "2026-07-15"},
        Order{"ORD-6", "S-001", "Customer", 10, OrderStatus::PRODUCING, "2026-07-15"},
        Order{"ORD-7", "S-001", "Customer", 10, OrderStatus::RELEASE, "2026-07-15"},
        Order{"ORD-8", "S-001", "Customer", 10, OrderStatus::REJECTED, "2026-07-15"},
        Order{"ORD-9", "S-001", "Customer", 10, OrderStatus::REJECTED, "2026-07-15"},
        Order{"ORD-10", "S-001", "Customer", 10, OrderStatus::REJECTED, "2026-07-15"},
        Order{"ORD-11", "S-001", "Customer", 10, OrderStatus::REJECTED, "2026-07-15"},
        Order{"ORD-12", "S-001", "Customer", 10, OrderStatus::REJECTED, "2026-07-15"},
    };

    OrderStatusCounts counts = CountOrdersByStatus(orders);

    EXPECT_EQ(counts.reserved, 2);
    EXPECT_EQ(counts.confirmed, 1);
    EXPECT_EQ(counts.producing, 3);
    EXPECT_EQ(counts.release, 1);
}

TEST(CountOrdersByStatusTest, MultipleSampleIdsAreAggregatedRegardlessOfSample) {
    std::vector<Order> orders{
        Order{"ORD-1", "S-001", "CustomerA", 10, OrderStatus::RESERVED, "2026-07-15"},
        Order{"ORD-2", "S-002", "CustomerB", 3, OrderStatus::RESERVED, "2026-07-15"},
        Order{"ORD-3", "S-003", "CustomerC", 7, OrderStatus::RESERVED, "2026-07-15"},
    };

    OrderStatusCounts counts = CountOrdersByStatus(orders);

    EXPECT_EQ(counts.reserved, 3);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);
}

}  // namespace
