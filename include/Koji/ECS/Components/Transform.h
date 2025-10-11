#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Koji {

struct Transform {
    glm::vec3 position  {0.0f, 0.0f, 0.0f};
    glm::quat rotation  {1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale     {1.f, 1.f, 1.f};

    [[nodiscard]] glm::mat4 GetMatrix() const {
        // Convert the quaternion to a 4x4 rotation matrix
        auto mat = glm::mat4(1.0f);
        mat =  glm::translate(mat, position);
        mat *= glm::toMat4(rotation);
        mat =  glm::scale(mat, scale);
        return mat;
    }
};

} // Koji