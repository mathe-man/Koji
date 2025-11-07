#pragma once
#include "Koji/ECS/Component.h"

struct kSphere : public kComponent<kSphere>
{
    float radius = 3;
    int slice = 10;
    int rings = 10;
    
    kSphere() = default;
    kSphere(const float& p_radius, const int& p_slice, const int& p_rings) 
        : radius(p_radius), slice(p_slice), rings(p_rings) {}
};