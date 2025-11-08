#pragma once
#include "Koji/ECS/Component.h"

struct Vec3 { float x, y, z; };
struct Quat { float x, y, z, w; };
namespace Koji::ECS
{
    struct kTransform : public kComponent<kTransform> {
        Vec3 position;
        Quat rotation;
        Vec3 scale;
    
        kTransform() = default;
        kTransform(const Vec3& p, const Quat& r, const Vec3& s) 
            : position(p), rotation(r), scale(s) {}
    };
}