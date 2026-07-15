#pragma once

#include <string>
#include <vector>

#include "OrderTypes.h"
#include "StockTypes.h"

namespace monitor {

// OrderStatusCounts를 사람이 읽을 수 있는 문자열로 렌더링한다.
// REJECTED는 OrderStatusCounts에 필드 자체가 없으므로 출력에도 나타나지 않는다.
// std::cout을 직접 호출하지 않는 순수 문자열 생성 함수다.
std::string RenderOrderCounts(const OrderStatusCounts& counts);

// 시료별 재고/수요/판정(여유/부족/고갈)을 사람이 읽을 수 있는 문자열로 렌더링한다.
// 목록이 비어 있으면 "데이터 없음" 안내 문구를 반환한다.
// std::cout을 직접 호출하지 않는 순수 문자열 생성 함수다.
std::string RenderStockLevels(const std::vector<StockLevel>& levels);

}  // namespace monitor
