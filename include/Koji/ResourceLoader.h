#pragma once
#include <unordered_map>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "bimg/bimg.h"
#include "ECS/Components/Model.h"

namespace Koji
{
    using namespace ECS;

    class ResourceLoader {
    private:
        Assimp::Importer importer;
        bimg::ImageContainer* (*imageParseFn)(bx::AllocatorI*, const void*, uint32_t);
        bx::AllocatorI* allocator = nullptr;
        
        // Caches
        std::unordered_map<std::string, std::shared_ptr<Model>> modelCache;
        std::unordered_map<std::string, bgfx::TextureHandle> textureCache;
        std::unordered_map<std::string, bgfx::ProgramHandle> shaderCache;
        
        // Assets directory
        std::string assetsDirectory = "assets/";

    public:
        static ResourceLoader& GetInstance();
        
        // Model load
        std::shared_ptr<Model> LoadModel(const std::string& filepath);
        void UnloadModel(const std::string& filepath);
        
        // Texture load
        bgfx::TextureHandle LoadTexture(const std::string& filepath);
        void UnloadTexture(const std::string& filepath);
        
        // Shaders load
        bgfx::ProgramHandle LoadShader(const std::string& vsPath, const std::string& fsPath);
        
        void Cleanup();
        
        void SetAssetsDirectory(const std::string& dir) { assetsDirectory = dir; }

    private:
        ResourceLoader() = default;
        ~ResourceLoader();
        
        // Internal methods for GLB files
        bool ProcessGLBFile(const std::string& filepath, ECS::Model& model);
        void ProcessNode(aiNode* node, const aiScene* scene, ECS::Model& model);
        ECS::Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
        ECS::Material ProcessMaterial(aiMaterial* material, const std::string& baseDir);
        void SetupBgfxBuffers(ECS::Mesh& mesh);
        
        
        std::string GetTexturePath(aiMaterial* material, aiTextureType type, const std::string& baseDir);
        bgfx::TextureHandle LoadTextureInternal(const std::string& filepath);
        
        // bimg allocators
        // static void* imageAlloc(bx::AllocatorI* _allocator, size_t _size, size_t _align, const char* _file, uint32_t _line);
        // static void imageFree(bx::AllocatorI* _allocator, void* _ptr, size_t _align, const char* _file, uint32_t _line);
    };
}
