#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../Component.h"
#include "Transform.h"
#include "Material.h"


namespace Koji::ECS
{
    struct Mesh {
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texCoords;
        std::vector<uint16_t> indices;
    };
}