#include "../../include/Koji/Render/GraphicsResourcesLoader.h"

#include <fstream>
#include <ios>
#include <vector>

using namespace Koji;

struct GraphicsResourcesLoader::PosColorVertex {
    float x, y, z;
    uint32_t abgr;

    static bgfx::VertexLayout init() {
        static bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .end();
        return layout;
    }
};

Mesh GraphicsResourcesLoader::CreateCube() {
    // Init vertex layout once
    PosColorVertex::init();

    // Define cube vertices (8 corners)
    static PosColorVertex vertices[] = {
        {-1.0f,  1.0f,  1.0f, 0xff0000ff}, // 0
        { 1.0f,  1.0f,  1.0f, 0xff00ff00}, // 1
        {-1.0f, -1.0f,  1.0f, 0xffff0000}, // 2
        { 1.0f, -1.0f,  1.0f, 0xffffffff}, // 3
        {-1.0f,  1.0f, -1.0f, 0xff000000}, // 4
        { 1.0f,  1.0f, -1.0f, 0xff00ffff}, // 5
        {-1.0f, -1.0f, -1.0f, 0xffff00ff}, // 6
        { 1.0f, -1.0f, -1.0f, 0xffffff00}, // 7
    };

    static const uint16_t indices[] = {
        0, 1, 2, 1, 3, 2,
        4, 6, 5, 5, 6, 7,
        0, 2, 4, 4, 2, 6,
        1, 5, 3, 5, 7, 3,
        0, 4, 1, 4, 5, 1,
        2, 3, 6, 6, 3, 7,
    };

    Mesh mesh{};
    mesh.vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices, sizeof(vertices)),
        PosColorVertex::init()
    );

    mesh.ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(indices, sizeof(indices))
    );

    return mesh;
}

bgfx::ShaderHandle GraphicsResourcesLoader::LoadShader(const std::string &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("Failed to open shader file: " + path);

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size))
        throw std::runtime_error("Failed to read shader file: " + path);

    const bgfx::Memory* mem = bgfx::copy(buffer.data(), static_cast<uint32_t>(size));
    bgfx::ShaderHandle handle = bgfx::createShader(mem);
    bgfx::setName(handle, path.c_str());
    return handle;
}

bgfx::ProgramHandle GraphicsResourcesLoader::LoadProgram(const std::string &vsPath, const std::string &fsPath) {
    bgfx::ShaderHandle vsh = LoadShader(vsPath);
    bgfx::ShaderHandle fsh = LoadShader(fsPath);
    return bgfx::createProgram(vsh, fsh, true); // true mean destroying the shaders when the program is destroyed
}

