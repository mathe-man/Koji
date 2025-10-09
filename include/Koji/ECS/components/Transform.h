#pragma once
#include <glm/glm.hpp>

namespace Koji {

struct Transform {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale{1.f, 1.f, 1.f};
};

} // Koji