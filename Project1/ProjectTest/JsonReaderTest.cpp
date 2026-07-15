#include "pch.h"
#include "../Project1/JsonReader.h"

#include <stdexcept>

namespace {

using monitor::LoadOrders;
using monitor::LoadSamples;
using monitor::Order;
using monitor::OrderStatus;
using monitor::Sample;

TEST(JsonReaderTest, LoadSamplesParsesValidFileWithCorrectFields) {
    std::vector<Sample> samples = LoadSamples("fixtures/samples_valid.json");

    ASSERT_EQ(samples.size(), 2u);
    EXPECT_EQ(samples[0].sampleId, "S-001");
    EXPECT_EQ(samples[0].name, "SampleA");
    EXPECT_EQ(samples[0].avgProductionTime, 30);
    EXPECT_DOUBLE_EQ(samples[0].yield, 0.9);
    EXPECT_EQ(samples[0].stock, 12);
    EXPECT_EQ(samples[1].sampleId, "S-002");
    EXPECT_EQ(samples[1].stock, 0);
}

TEST(JsonReaderTest, LoadOrdersParsesValidFileWithCorrectFieldsAndStatuses) {
    std::vector<Order> orders = LoadOrders("fixtures/orders_valid.json");

    ASSERT_EQ(orders.size(), 3u);
    EXPECT_EQ(orders[0].orderId, "ORD-20260416-0001");
    EXPECT_EQ(orders[0].sampleId, "S-001");
    EXPECT_EQ(orders[0].customerName, "CustomerA");
    EXPECT_EQ(orders[0].quantity, 5);
    EXPECT_EQ(orders[0].status, OrderStatus::RESERVED);
    EXPECT_EQ(orders[0].createdAt, "2026-04-16T10:00:00");
    EXPECT_EQ(orders[1].status, OrderStatus::CONFIRMED);
    EXPECT_EQ(orders[2].status, OrderStatus::REJECTED);
}

TEST(JsonReaderTest, LoadOrdersMapsAllStatusStringsToEnum) {
    std::vector<Order> orders = LoadOrders("fixtures/orders_all_statuses.json");

    ASSERT_EQ(orders.size(), 5u);
    EXPECT_EQ(orders[0].status, OrderStatus::RESERVED);
    EXPECT_EQ(orders[1].status, OrderStatus::REJECTED);
    EXPECT_EQ(orders[2].status, OrderStatus::PRODUCING);
    EXPECT_EQ(orders[3].status, OrderStatus::CONFIRMED);
    EXPECT_EQ(orders[4].status, OrderStatus::RELEASE);
}

TEST(JsonReaderTest, LoadOrdersUnknownStatusStringThrows) {
    EXPECT_THROW(LoadOrders("fixtures/orders_unknown_status.json"), std::runtime_error);
}

TEST(JsonReaderTest, LoadSamplesFileDoesNotExistThrows) {
    EXPECT_THROW(LoadSamples("fixtures/does_not_exist.json"), std::runtime_error);
}

TEST(JsonReaderTest, LoadOrdersFileDoesNotExistThrows) {
    EXPECT_THROW(LoadOrders("fixtures/does_not_exist.json"), std::runtime_error);
}

TEST(JsonReaderTest, LoadSamplesEmptyFileReturnsEmptyVector) {
    std::vector<Sample> samples = LoadSamples("fixtures/samples_empty.json");

    EXPECT_TRUE(samples.empty());
}

TEST(JsonReaderTest, LoadOrdersEmptyFileReturnsEmptyVector) {
    std::vector<Order> orders = LoadOrders("fixtures/orders_empty.json");

    EXPECT_TRUE(orders.empty());
}

}  // namespace
