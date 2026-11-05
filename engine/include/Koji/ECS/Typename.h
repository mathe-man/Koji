#pragma once
#include <string>

namespace Koji::ECS
{
    // Return a readable typename (typeid(T).name()).
    std::string Demangle(const char* mangledName);
}