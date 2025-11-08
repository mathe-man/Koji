#pragma once
#include <vector>
#include "World.h"

namespace Koji::Engine
{
    class System {
    public:
        virtual ~System() = default;
        virtual bool Init()     { return true; }
        virtual bool Update()   { return true; }
        virtual bool Close()   { return true; }
    };
}