#include "MonitorSnapshot.h"

#include "JsonReader.h"
#include "MonitorAssembly.h"
#include "OrderAggregation.h"

namespace monitor {

MonitorSnapshot RefreshSnapshot(const std::string& dataDir) {
    std::vector<Sample> samples = LoadSamples(dataDir + "/samples.json");
    std::vector<Order> orders = LoadOrders(dataDir + "/orders.json");

    MonitorSnapshot snapshot;
    snapshot.orderCounts = CountOrdersByStatus(orders);
    snapshot.stockLevels = BuildStockLevels(samples, orders);
    return snapshot;
}

}  // namespace monitor
