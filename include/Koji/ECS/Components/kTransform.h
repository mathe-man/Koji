#pragma once
#include <raylib.h>


struct kTransform {
    Vector3 position  {0.0f, 0.0f, 0.0f};
    Vector4 rotation  {1.0f, 0.0f, 0.0f, 0.0f};
    Vector3 scale     {1.f, 1.f, 1.f};
};