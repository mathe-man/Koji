#pragma once
#include <glm/glm.hpp>

namespace Koji {

struct Velocity {
    glm::vec3 LinearVelocity;
    glm::quat AngularVelocity;
};

} // Koji