// Monitor 콘솔 진입점.
// data/samples.json, data/orders.json을 읽어(JsonReader) 조립하고
// (MonitorAssembly/OrderAggregation) 렌더링(ConsoleRenderer)해서 콘솔에 출력한다.
// 이 파일은 조립/실행 진입점이며, 로직은 전부 각 모듈에 위임한다.
#include <iostream>
#include <string>

#include "ConsoleRenderer.h"
#include "MonitorSnapshot.h"

namespace {

void PrintSnapshot(const monitor::MonitorSnapshot& snapshot) {
    std::cout << "\n=== 주문량 확인 (상태별 건수, REJECTED 제외) ===\n";
    std::cout << monitor::RenderOrderCounts(snapshot.orderCounts);
    std::cout << "\n=== 재고량 확인 (시료별 재고/수요/판정) ===\n";
    std::cout << monitor::RenderStockLevels(snapshot.stockLevels);
}

}  // namespace

int main(int argc, char** argv) {
    std::string dataDir = argc > 1 ? argv[1] : "data";

    std::cout << "Monitor - 데이터 모니터링 Tool (읽기 전용)\n";
    std::cout << "데이터 디렉토리: " << dataDir << "\n";

    try {
        monitor::MonitorSnapshot snapshot = monitor::RefreshSnapshot(dataDir);
        PrintSnapshot(snapshot);
    } catch (const std::exception& e) {
        std::cout << "데이터 로딩 실패: " << e.what() << "\n";
    }

    std::cout << "\n명령: r(갱신) / q(종료)\n";
    std::string command;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, command)) {
            break;
        }
        if (command == "q") {
            break;
        }
        if (command == "r" || command.empty()) {
            try {
                monitor::MonitorSnapshot snapshot = monitor::RefreshSnapshot(dataDir);
                PrintSnapshot(snapshot);
            } catch (const std::exception& e) {
                std::cout << "데이터 로딩 실패: " << e.what() << "\n";
            }
        }
    }

    return 0;
}
