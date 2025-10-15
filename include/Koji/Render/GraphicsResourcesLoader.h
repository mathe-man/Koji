#pragma once
#include <string>

#include "Koji/ECS/Components/Mesh.h"

namespace Koji {
    class GraphicsResourcesLoader {
    public:
        // Struct with a 3D position and 4 channel color
        struct PosColorVertex;

        static Mesh CreateCube();

        // Loads a precompiled bgfx shader (.bin) from disk
        static bgfx::ShaderHandle LoadShader(const std::string& path);

        // Create a bgfx program (vertex + fragment)
        static bgfx::ProgramHandle LoadProgram(const std::string& vsPath, const std::string& fsPath);
    };
} // Koji