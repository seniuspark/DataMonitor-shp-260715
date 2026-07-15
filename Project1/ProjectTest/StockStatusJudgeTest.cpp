#include "pch.h"
#include "../Project1/StockStatusJudge.h"

namespace {

using monitor::JudgeStockStatus;
using monitor::StockLevel;
using monitor::StockStatus;

// stock == 0 규칙이 최우선이라는 점을 강조하기 위해 demand == 0 케이스도 DEPLETED다.
TEST(JudgeStockStatusTest, ZeroStockAndZeroDemandIsDepleted) {
    EXPECT_EQ(JudgeStockStatus(0, 0), StockStatus::DEPLETED);
}

TEST(JudgeStockStatusTest, ZeroStockWithPositiveDemandIsDepleted) {
    EXPECT_EQ(JudgeStockStatus(0, 10), StockStatus::DEPLETED);
}

TEST(JudgeStockStatusTest, PositiveStockWithNoDemandIsSufficient) {
    EXPECT_EQ(JudgeStockStatus(5, 0), StockStatus::SUFFICIENT);
}

TEST(JudgeStockStatusTest, StockBelowDemandIsShortage) {
    EXPECT_EQ(JudgeStockStatus(3, 10), StockStatus::SHORTAGE);
}

// 핵심 경계 테스트: 재고와 수요가 정확히 같으면 "부족"이 아니라 "여유"로 판정한다
// (재고가 수요를 정확히 충당할 수 있으므로).
TEST(JudgeStockStatusTest, StockExactlyEqualToDemandIsSufficient) {
    EXPECT_EQ(JudgeStockStatus(10, 10), StockStatus::SUFFICIENT);
}

TEST(JudgeStockStatusTest, StockAboveDemandIsSufficient) {
    EXPECT_EQ(JudgeStockStatus(10, 3), StockStatus::SUFFICIENT);
}

// 방어적 clamp 정책: 도메인상 발생해서는 안 되는 음수 입력은 0으로 취급한다.
TEST(JudgeStockStatusTest, NegativeStockIsClampedToZeroAndTreatedAsDepleted) {
    EXPECT_EQ(JudgeStockStatus(-5, 10), StockStatus::DEPLETED);
}

TEST(JudgeStockStatusTest, NegativeDemandIsClampedToZero) {
    EXPECT_EQ(JudgeStockStatus(5, -3), StockStatus::SUFFICIENT);
}

TEST(JudgeStockStatusTest, StockLevelOverloadDelegatesToStockAndDemand) {
    StockLevel level{"S-001", 3, 10};

    EXPECT_EQ(JudgeStockStatus(level), StockStatus::SHORTAGE);
}

}  // namespace
