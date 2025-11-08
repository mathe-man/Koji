#pragma once
#include <string>
#include "Koji/ECS/Component.h"

namespace Koji::ECS
{
    struct MetaData : public kComponent<MetaData> {
        std::string name;
        int layer{};
        bool active{};

        MetaData() = default;
        MetaData(const std::string& n, int l, bool a)
            : name(n), layer(l), active(a) {}
    };
}