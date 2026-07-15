#pragma once

#include <string>
#include <vector>

#include "OrderTypes.h"
#include "SampleTypes.h"

namespace monitor {

// Json/ PoC가 확정한 스키마(data/samples.json, data/orders.json)를 읽어
// Sample/Order 구조체 목록으로 파싱한다. 읽기 전용 — 파일에 쓰지 않는다.
//
// 정책:
//   - 파일이 존재하지 않으면 std::runtime_error를 던진다.
//   - status 필드가 알려진 값(RESERVED/REJECTED/PRODUCING/CONFIRMED/RELEASE)이
//     아니면 std::runtime_error를 던진다 (무시하지 않고 실패를 드러낸다).
//   - 목록이 비어 있으면 빈 벡터를 반환한다.
std::vector<Sample> LoadSamples(const std::string& path);
std::vector<Order> LoadOrders(const std::string& path);

}  // namespace monitor
