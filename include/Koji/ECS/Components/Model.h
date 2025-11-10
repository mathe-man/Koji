#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../Component.h"
#include "Transform.h"
#include <bgfx/bgfx.h>

namespace Koji::ECS
{
    struct Mesh {
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texCoords;
        std::vector<uint16_t> indices;
        
        bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle indexBuffer = BGFX_INVALID_HANDLE;
        bgfx::VertexLayout vertexLayout;
        uint32_t materialIndex = 0;
    };
    
    struct Material {
        struct Color {
            float r, g, b, a;
            Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) 
                : r(r), g(g), b(b), a(a) {}
        };
        
        Color diffuseColor;
        Color specularColor;
        float shininess = 32.0f;
        
        // textures references, managed by the ResourceLoader
        std::string diffuseTexturePath;
        std::string normalTexturePath;
        std::string specularTexturePath;
        
        // Bgfx handles updated by the ResourceLoader
        bgfx::TextureHandle diffuseTextureHandle = BGFX_INVALID_HANDLE;
        bgfx::TextureHandle normalTextureHandle = BGFX_INVALID_HANDLE;
        bgfx::TextureHandle specularTextureHandle = BGFX_INVALID_HANDLE;
        
        bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    };
    
    struct Model : public kComponent<Model> {
        std::vector<Mesh> meshes;
        std::vector<Material> materials;
        bool loaded = false;
        bool visible = true;
        
        ~Model() = default;
    };
}
