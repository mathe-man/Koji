#include <Koji/ECS/Components/Mesh.h>
using namespace Koji;

struct MeshCreator::PosColorVertex {
    float x, y, z;
    uint32_t abgr;

    static void init() {
        static bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .end();
    }
};

Mesh MeshCreator::CreateCube() {
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
