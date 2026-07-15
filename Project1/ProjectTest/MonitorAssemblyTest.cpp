#include "pch.h"
#include "../Project1/MonitorAssembly.h"

#include "../Project1/JsonReader.h"
#include "../Project1/OrderAggregation.h"
#include "../Project1/StockStatusJudge.h"

namespace {

using monitor::BuildStockLevels;
using monitor::CountOrdersByStatus;
using monitor::JudgeStockStatus;
using monitor::LoadOrders;
using monitor::LoadSamples;
using monitor::Order;
using monitor::OrderStatus;
using monitor::OrderStatusCounts;
using monitor::Sample;
using monitor::StockLevel;
using monitor::StockStatus;

TEST(MonitorAssemblyTest, DemandSumsReservedConfirmedProducingButExcludesReleaseAndRejected) {
    std::vector<Sample> samples{Sample{"S-001", "SampleA", 30, 0.9, 12}};
    std::vector<Order> orders{
        Order{"ORD-1", "S-001", "A", 3, OrderStatus::RESERVED, "t"},
        Order{"ORD-2", "S-001", "A", 4, OrderStatus::RESERVED, "t"},
        Order{"ORD-3", "S-001", "A", 5, OrderStatus::CONFIRMED, "t"},
        Order{"ORD-4", "S-001", "A", 100, OrderStatus::RELEASE, "t"},
        Order{"ORD-5", "S-001", "A", 999, OrderStatus::REJECTED, "t"},
    };

    std::vector<StockLevel> levels = BuildStockLevels(samples, orders);

    ASSERT_EQ(levels.size(), 1u);
    EXPECT_EQ(levels[0].sampleId, "S-001");
    EXPECT_EQ(levels[0].stock, 12);
    EXPECT_EQ(levels[0].demand, 12);
}

TEST(MonitorAssemblyTest, SampleWithNoOrdersHasZeroDemandAndIsSufficient) {
    std::vector<Sample> samples{Sample{"S-001", "SampleA", 30, 0.9, 5}};
    std::vector<Order> orders;

    std::vector<StockLevel> levels = BuildStockLevels(samples, orders);

    ASSERT_EQ(levels.size(), 1u);
    EXPECT_EQ(levels[0].demand, 0);
    EXPECT_EQ(JudgeStockStatus(levels[0]), StockStatus::SUFFICIENT);
}

// end-to-end: fixture 로딩 -> 조립 -> 집계/판정까지 연결되는지 검증.
TEST(MonitorAssemblyTest, EndToEndFixtureLoadAssembleAndJudge) {
    std::vector<Sample> samples = LoadSamples("fixtures/samples_valid.json");
    std::vector<Order> orders = LoadOrders("fixtures/orders_valid.json");

    std::vector<StockLevel> levels = BuildStockLevels(samples, orders);
    OrderStatusCounts counts = CountOrdersByStatus(orders);

    ASSERT_EQ(levels.size(), 2u);

    // 상태별 집계: REJECTED 1건은 제외되어야 한다.
    EXPECT_EQ(counts.reserved, 1);
    EXPECT_EQ(counts.confirmed, 1);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);

    // S-001: stock=12, demand=5(RESERVED)+3(CONFIRMED)=8 -> SUFFICIENT
    EXPECT_EQ(levels[0].sampleId, "S-001");
    EXPECT_EQ(levels[0].stock, 12);
    EXPECT_EQ(levels[0].demand, 8);
    EXPECT_EQ(JudgeStockStatus(levels[0]), StockStatus::SUFFICIENT);

    // S-002: stock=0, demand=0 (유일한 주문이 REJECTED라 제외됨) -> DEPLETED
    EXPECT_EQ(levels[1].sampleId, "S-002");
    EXPECT_EQ(levels[1].stock, 0);
    EXPECT_EQ(levels[1].demand, 0);
    EXPECT_EQ(JudgeStockStatus(levels[1]), StockStatus::DEPLETED);
}

// 빈 시료/주문 fixture -> Phase 1/2에서 이미 검증한 "빈 목록 -> 카운트 0"과 일관됨을 재확인.
TEST(MonitorAssemblyTest, EmptyFixturesYieldEmptyAggregatesConsistentWithPhase1And2) {
    std::vector<Sample> samples = LoadSamples("fixtures/samples_empty.json");
    std::vector<Order> orders = LoadOrders("fixtures/orders_empty.json");

    std::vector<StockLevel> levels = BuildStockLevels(samples, orders);
    OrderStatusCounts counts = CountOrdersByStatus(orders);

    EXPECT_TRUE(levels.empty());
    EXPECT_EQ(counts.reserved, 0);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);
}

}  // namespace
