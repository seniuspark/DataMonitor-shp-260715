#include "pch.h"
#include "../Project1/ConsoleRenderer.h"

#include "../Project1/OrderTypes.h"
#include "../Project1/StockTypes.h"

namespace {

using monitor::OrderStatusCounts;
using monitor::RenderOrderCounts;
using monitor::RenderStockLevels;
using monitor::StockLevel;

TEST(ConsoleRendererTest, RenderOrderCountsIncludesEachStatusAndCountButNotRejected) {
    OrderStatusCounts counts{/*reserved*/ 2, /*confirmed*/ 1, /*producing*/ 3, /*release*/ 1};

    std::string rendered = RenderOrderCounts(counts);

    EXPECT_NE(rendered.find("RESERVED"), std::string::npos);
    EXPECT_NE(rendered.find("2"), std::string::npos);
    EXPECT_NE(rendered.find("CONFIRMED"), std::string::npos);
    EXPECT_NE(rendered.find("1"), std::string::npos);
    EXPECT_NE(rendered.find("PRODUCING"), std::string::npos);
    EXPECT_NE(rendered.find("3"), std::string::npos);
    EXPECT_NE(rendered.find("RELEASE"), std::string::npos);
    EXPECT_EQ(rendered.find("REJECTED"), std::string::npos);
}

TEST(ConsoleRendererTest, RenderStockLevelsSufficientCaseIncludesLabelAndValues) {
    std::vector<StockLevel> levels{StockLevel{"S-001", 10, 3}};

    std::string rendered = RenderStockLevels(levels);

    EXPECT_NE(rendered.find("여유"), std::string::npos);
    EXPECT_NE(rendered.find("S-001"), std::string::npos);
    EXPECT_NE(rendered.find("10"), std::string::npos);
    EXPECT_NE(rendered.find("3"), std::string::npos);
}

TEST(ConsoleRendererTest, RenderStockLevelsShortageCaseIncludesLabel) {
    std::vector<StockLevel> levels{StockLevel{"S-002", 3, 10}};

    std::string rendered = RenderStockLevels(levels);

    EXPECT_NE(rendered.find("부족"), std::string::npos);
}

TEST(ConsoleRendererTest, RenderStockLevelsDepletedCaseIncludesLabel) {
    std::vector<StockLevel> levels{StockLevel{"S-003", 0, 5}};

    std::string rendered = RenderStockLevels(levels);

    EXPECT_NE(rendered.find("고갈"), std::string::npos);
}

TEST(ConsoleRendererTest, RenderStockLevelsMultipleSamplesEachLineMatchesItsOwnJudgement) {
    std::vector<StockLevel> levels{
        StockLevel{"S-001", 10, 3},   // 여유
        StockLevel{"S-002", 3, 10},   // 부족
        StockLevel{"S-003", 0, 5},    // 고갈
    };

    std::string rendered = RenderStockLevels(levels);

    size_t sufficientPos = rendered.find("여유");
    size_t shortagePos = rendered.find("부족");
    size_t depletedPos = rendered.find("고갈");

    ASSERT_NE(sufficientPos, std::string::npos);
    ASSERT_NE(shortagePos, std::string::npos);
    ASSERT_NE(depletedPos, std::string::npos);
    EXPECT_LT(sufficientPos, shortagePos);
    EXPECT_LT(shortagePos, depletedPos);
}

TEST(ConsoleRendererTest, RenderOrderCountsEmptyStillProducesNoDataMessageWithoutCrash) {
    OrderStatusCounts counts{};

    std::string rendered = RenderOrderCounts(counts);

    EXPECT_NE(rendered.find("0"), std::string::npos);
}

TEST(ConsoleRendererTest, RenderStockLevelsEmptyProducesNoDataMessageWithoutCrash) {
    std::vector<StockLevel> levels;

    std::string rendered = RenderStockLevels(levels);

    EXPECT_NE(rendered.find("데이터 없음"), std::string::npos);
}

}  // namespace
