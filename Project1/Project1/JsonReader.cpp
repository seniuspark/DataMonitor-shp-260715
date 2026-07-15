#include "JsonReader.h"

#include <fstream>
#include <stdexcept>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace monitor {

namespace {

nlohmann::json LoadJsonFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("파일을 열 수 없습니다: " + path);
    }

    nlohmann::json root;
    file >> root;
    return root;
}

OrderStatus ParseOrderStatus(const std::string& statusText) {
    static const std::unordered_map<std::string, OrderStatus> kStatusByText{
        {"RESERVED", OrderStatus::RESERVED},   {"REJECTED", OrderStatus::REJECTED},
        {"PRODUCING", OrderStatus::PRODUCING}, {"CONFIRMED", OrderStatus::CONFIRMED},
        {"RELEASE", OrderStatus::RELEASE},
    };

    auto it = kStatusByText.find(statusText);
    if (it == kStatusByText.end()) {
        throw std::runtime_error("알 수 없는 주문 상태 문자열: " + statusText);
    }
    return it->second;
}

}  // namespace

std::vector<Sample> LoadSamples(const std::string& path) {
    nlohmann::json root = LoadJsonFile(path);

    std::vector<Sample> samples;
    for (const auto& item : root.at("samples")) {
        samples.push_back(Sample{
            item.at("sampleId").get<std::string>(),
            item.at("name").get<std::string>(),
            item.at("avgProductionTime").get<int>(),
            item.at("yield").get<double>(),
            item.at("stock").get<int>(),
        });
    }
    return samples;
}

std::vector<Order> LoadOrders(const std::string& path) {
    nlohmann::json root = LoadJsonFile(path);

    std::vector<Order> orders;
    for (const auto& item : root.at("orders")) {
        orders.push_back(Order{
            item.at("orderId").get<std::string>(),
            item.at("sampleId").get<std::string>(),
            item.at("customerName").get<std::string>(),
            item.at("quantity").get<int>(),
            ParseOrderStatus(item.at("status").get<std::string>()),
            item.at("createdAt").get<std::string>(),
        });
    }
    return orders;
}

}  // namespace monitor
