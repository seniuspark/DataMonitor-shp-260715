#include "pch.h"
#include "../Project1/MonitorSnapshot.h"

#include <filesystem>
#include <fstream>

namespace {

using monitor::MonitorSnapshot;
using monitor::RefreshSnapshot;
using monitor::StockStatus;

TEST(MonitorSnapshotTest, RefreshSnapshotLoadsCountsAndStockLevelsFromDataDir) {
    MonitorSnapshot snapshot = RefreshSnapshot("fixtures/monitor_snapshot");

    EXPECT_EQ(snapshot.orderCounts.reserved, 1);
    EXPECT_EQ(snapshot.orderCounts.confirmed, 1);
    EXPECT_EQ(snapshot.orderCounts.producing, 0);
    EXPECT_EQ(snapshot.orderCounts.release, 0);

    ASSERT_EQ(snapshot.stockLevels.size(), 2u);
    EXPECT_EQ(snapshot.stockLevels[0].sampleId, "S-001");
    EXPECT_EQ(snapshot.stockLevels[0].stock, 12);
    EXPECT_EQ(snapshot.stockLevels[0].demand, 8);
    EXPECT_EQ(snapshot.stockLevels[1].sampleId, "S-002");
    EXPECT_EQ(snapshot.stockLevels[1].stock, 0);
    EXPECT_EQ(snapshot.stockLevels[1].demand, 0);
}

// 데이터 파일을 다시 로딩할 때마다 최신 내용을 반영하는지 확인 (갱신 동작의 근거).
TEST(MonitorSnapshotTest, RefreshSnapshotRereadsFileEachCallReflectingLatestData) {
    const std::string dir = "fixtures/monitor_snapshot_refresh";
    std::filesystem::create_directories(dir);

    auto writeSamples = [&](int stock) {
        std::ofstream out(dir + "/samples.json");
        out << "{ \"samples\": [ { \"sampleId\": \"S-001\", \"name\": \"SampleA\", "
               "\"avgProductionTime\": 30, \"yield\": 0.9, \"stock\": "
            << stock << " } ] }";
    };
    std::ofstream ordersOut(dir + "/orders.json");
    ordersOut << "{ \"orders\": [] }";
    ordersOut.close();

    writeSamples(5);
    MonitorSnapshot first = RefreshSnapshot(dir);
    EXPECT_EQ(first.stockLevels[0].stock, 5);

    writeSamples(9);
    MonitorSnapshot second = RefreshSnapshot(dir);
    EXPECT_EQ(second.stockLevels[0].stock, 9);
}

}  // namespace
