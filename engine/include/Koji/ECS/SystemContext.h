#pragma once
#include <Koji/ECS/SystemRegistry.h>

namespace Koji::ECS
{
    // Every system receive one on each call
    struct SystemContext
    {
        SystemRegistry* systems = nullptr;


        template <typename T>
        T* GetSystem() const { return systems ? systems->GetSystem<T>() : nullptr; }
    };
}