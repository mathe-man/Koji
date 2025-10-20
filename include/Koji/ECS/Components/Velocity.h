#pragma once
#include <glm/glm.hpp>

#include "raylib.h"

namespace Koji {

struct Velocity {
    Vector3 LinearVelocity;
    Vector4 AngularVelocity;
};

} // Koji