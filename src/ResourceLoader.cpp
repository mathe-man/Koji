#include "Koji/ResourceLoader.h"
#include <bgfx/bgfx.h>
#include <bimg/decode.h>
#include <bx/file.h>
#include <iostream>
#include <fstream>

#include "assimp/postprocess.h"

using namespace Koji;
using namespace Koji::ECS;

ResourceLoader& ResourceLoader::GetInstance() {
    static ResourceLoader instance;
    return instance;
}

ResourceLoader::~ResourceLoader() {
    Cleanup();
}

std::shared_ptr<Model> ResourceLoader::LoadModel(const std::string& filepath) {
    std::string fullPath = assetsDirectory + filepath;
    
    // Check the cache
    auto it = modelCache.find(fullPath);
    if (it != modelCache.end()) {
        std::cout << "Model loaded from cache: " << filepath << std::endl;
        return it->second;
    }
    
    // Create a new model
    auto model = std::make_shared<Model>();
    
    if (ProcessGLBFile(fullPath, *model)) {
        // Load textures for the materials
        for (auto& material : model->materials) {
            if (!material.diffuseTexturePath.empty()) {
                material.diffuseTextureHandle = LoadTexture(material.diffuseTexturePath);
            }
            if (!material.normalTexturePath.empty()) {
                material.normalTextureHandle = LoadTexture(material.normalTexturePath);
            }
            if (!material.specularTexturePath.empty()) {
                material.specularTextureHandle = LoadTexture(material.specularTexturePath);
            }
        }
        
        model->loaded = true;
        modelCache[fullPath] = model;
        
        std::cout << "Model loaded successfully: " << filepath 
                  << " (" << model->meshes.size() << " meshes)" << std::endl;
        
        return model;
    }
    
    std::cout << "Failed to load model: " << filepath << std::endl;
    return nullptr;
}

bool ResourceLoader::ProcessGLBFile(const std::string& filepath, Model& model) {
    std::string baseDirectory = filepath.substr(0, filepath.find_last_of("/\\")) + "/";

    unsigned int flags = 
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality;

    const aiScene* scene = importer.ReadFile(filepath, flags);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // Apply materials
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        model.materials.push_back(ProcessMaterial(scene->mMaterials[i], baseDirectory));
    }

    // Apply scene order
    ProcessNode(scene->mRootNode, scene, model);

    // Init bgfx buffers
    for (auto& mesh : model.meshes) {
        SetupBgfxBuffers(mesh);
    }

    return true;
}

void ResourceLoader::ProcessNode(aiNode* node, const aiScene* scene, Model& model) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model.meshes.push_back(ProcessMesh(mesh, scene));
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, model);
    }
}

Mesh ResourceLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    Mesh result;
    
    // Vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // Position
        result.vertices.push_back(mesh->mVertices[i].x);
        result.vertices.push_back(mesh->mVertices[i].y);
        result.vertices.push_back(mesh->mVertices[i].z);
        
        // Normals
        if (mesh->HasNormals()) {
            result.normals.push_back(mesh->mNormals[i].x);
            result.normals.push_back(mesh->mNormals[i].y);
            result.normals.push_back(mesh->mNormals[i].z);
        }
        
        // Tex coords
        if (mesh->mTextureCoords[0]) {
            result.texCoords.push_back(mesh->mTextureCoords[0][i].x);
            result.texCoords.push_back(mesh->mTextureCoords[0][i].y);
        }
    }
    
    // Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            result.indices.push_back(face.mIndices[j]);
        }
    }
    
    // Material index
    result.materialIndex = mesh->mMaterialIndex;
    
    return result;
}

Material ResourceLoader::ProcessMaterial(aiMaterial* material, const std::string& baseDir) {
    Material mat;
    aiColor3D color;
    float value;
    
    // Material properties
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        mat.diffuseColor = { color.r, color.g, color.b, 1.0f };
    }
    
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        mat.specularColor = { color.r, color.g, color.b, 1.0f };
    }
    
    if (material->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
        mat.shininess = value;
    }
    
    // Textures path
    mat.diffuseTexturePath = GetTexturePath(material, aiTextureType_DIFFUSE, baseDir);
    mat.normalTexturePath = GetTexturePath(material, aiTextureType_NORMALS, baseDir);
    mat.specularTexturePath = GetTexturePath(material, aiTextureType_SPECULAR, baseDir);
    
    return mat;
}

void ResourceLoader::SetupBgfxBuffers(Mesh& mesh) {
    // Vertices layout
    mesh.vertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
    
    // Vertex buffer
    const bgfx::Memory* vertexMem = bgfx::copy(
        mesh.vertices.data(), 
        mesh.vertices.size() * sizeof(float)
    );
    mesh.vertexBuffer = bgfx::createVertexBuffer(vertexMem, mesh.vertexLayout);
    
    // Index buffer
    const bgfx::Memory* indexMem = bgfx::copy(
        mesh.indices.data(), 
        mesh.indices.size() * sizeof(uint16_t)
    );
    mesh.indexBuffer = bgfx::createIndexBuffer(indexMem);
}

bgfx::TextureHandle ResourceLoader::LoadTexture(const std::string& filepath) {
    std::string fullPath = assetsDirectory + filepath;
    
    // Check chache
    auto it = textureCache.find(fullPath);
    if (it != textureCache.end()) {
        return it->second;
    }
    
    bgfx::TextureHandle texture = LoadTextureInternal(fullPath);
    if (bgfx::isValid(texture)) {
        textureCache[fullPath] = texture;
        std::cout << "Texture loaded: " << filepath << std::endl;
    }
    
    return texture;
}

// bgfx file loading
static void* loadMemory(const char* filepath, uint32_t* size) {
    bx::FileReader reader;
    if (bx::open(&reader, filepath)) {
        uint32_t dataSize = (uint32_t)bx::getSize(&reader);
        void* data = malloc(dataSize);
        
        bx::Error err;
        int32_t bytesRead = bx::read(&reader, data, dataSize, &err);
        
        bx::close(&reader);
        
        if (bytesRead == dataSize) {
            if (size) *size = dataSize;
            return data;
        } else {
            free(data);
            return nullptr;
        }
    }
    return nullptr;
}

std::string ResourceLoader::GetTexturePath(aiMaterial* material, aiTextureType type, const std::string& baseDir) {
    if (material->GetTextureCount(type) > 0) {
        aiString path;
        material->GetTexture(type, 0, &path);
        
        // Nettoyer le chemin si nécessaire
        std::string texturePath = path.C_Str();
        
        // Retirer les préfixes de chemin si présents
        size_t pos = texturePath.find("textures/");
        if (pos != std::string::npos) {
            texturePath = texturePath.substr(pos);
        }
        
        return baseDir + texturePath;
    }
    return "";
}

bgfx::TextureHandle ResourceLoader::LoadTextureInternal(const std::string& filepath) {
    uint32_t textureSize;
    void* textureData = loadMemory(filepath.c_str(), &textureSize);
    
    if (!textureData) {
        std::cout << "Failed to load texture file: " << filepath << std::endl;
        return BGFX_INVALID_HANDLE;
    }
    
    // Decode image
    bimg::ImageContainer* imageContainer = bimg::imageParse(
        allocator, 
        textureData, 
        textureSize
    );
    
    free(textureData);
    
    if (!imageContainer) {
        std::cout << "Failed to parse texture: " << filepath << std::endl;
        return BGFX_INVALID_HANDLE;
    }
    
    // Create bgfx texture
    const bgfx::Memory* mem = bgfx::makeRef(
        imageContainer->m_data,
        imageContainer->m_size,
        [](void* ptr, void* userData) {
            bimg::ImageContainer* ic = (bimg::ImageContainer*)userData;
            bimg::imageFree(ic);
        },
        imageContainer
    );
    
    bgfx::TextureHandle texture = bgfx::createTexture2D(
        uint16_t(imageContainer->m_width),
        uint16_t(imageContainer->m_height),
        1 < imageContainer->m_numMips,
        imageContainer->m_numLayers,
        bgfx::TextureFormat::Enum(imageContainer->m_format),
        BGFX_TEXTURE_NONE | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
        mem
    );
    
    if (!bgfx::isValid(texture)) {
        std::cout << "Failed to create bgfx texture: " << filepath << std::endl;
        bimg::imageFree(imageContainer);
        return BGFX_INVALID_HANDLE;
    }
    
    std::cout << "Texture loaded successfully: " << filepath 
              << " (" << imageContainer->m_width << "x" << imageContainer->m_height << ")" 
              << std::endl;
    
    return texture;
}

void ResourceLoader::Cleanup() {
    // Clean models
    for (auto& [path, model] : modelCache) {
        for (auto& mesh : model->meshes) {
            if (bgfx::isValid(mesh.vertexBuffer)) {
                bgfx::destroy(mesh.vertexBuffer);
            }
            if (bgfx::isValid(mesh.indexBuffer)) {
                bgfx::destroy(mesh.indexBuffer);
            }
        }
    }
    modelCache.clear();
    
    // Clean textures
    for (auto& [path, texture] : textureCache) {
        if (bgfx::isValid(texture)) {
            bgfx::destroy(texture);
        }
    }
    textureCache.clear();
    
    // Clean shaders
    for (auto& [path, program] : shaderCache) {
        if (bgfx::isValid(program)) {
            bgfx::destroy(program);
        }
    }
    shaderCache.clear();
}