#pragma once

#include <string>

namespace monitor {

struct Sample {
    std::string sampleId;
    std::string name;
    int avgProductionTime;
    double yield;
    int stock;
};

}  // namespace monitor
