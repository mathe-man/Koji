#pragma once
#include <glm/glm.hpp>

namespace Koji {

struct Transform {
    glm::vec3 position{0.0F};
    glm::quat rotation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
};

} // Koji