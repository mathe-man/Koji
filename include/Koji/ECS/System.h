#pragma once
#include <vector>
#include "World.h"

class System {
public:
    virtual ~System() = default;
    virtual void Update() = 0;
};