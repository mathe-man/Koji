#pragma once
#include <bgfx/bgfx.h>

namespace Koji {

    struct Mesh {
        bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle  ibh = BGFX_INVALID_HANDLE;
        bgfx::ProgramHandle program  = BGFX_INVALID_HANDLE;
    };

} // Koji