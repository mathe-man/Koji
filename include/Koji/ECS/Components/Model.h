#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Koji/ECS/Component.h"

namespace Koji::ECS
{
    
    // Structure pour les matériaux PBR
    struct Material {
        glm::vec3 albedo = glm::vec3(1.0f, 1.0f, 1.0f);
        float metallic = 0.0f;
        float roughness = 0.5f;
        float ao = 1.0f;
        float alpha = 1.0f;
        
        // Références aux textures (seront utilisées plus tard)
        unsigned int albedoTexture = 0;
        unsigned int normalTexture = 0;
        unsigned int metallicTexture = 0;
        unsigned int roughnessTexture = 0;
        
        Material() = default;
        Material(const glm::vec3& color, float metal = 0.0f, float rough = 0.5f) 
            : albedo(color), metallic(metal), roughness(rough) {}
    };

    // Mesh - représente une partie du modèle avec son propre matériau
    struct Mesh {
        unsigned int VAO = 0;
        unsigned int vertexCount = 0;
        unsigned int indexCount = 0;
        Material material;
        glm::vec3 bboxMin = glm::vec3(0.0f);
        glm::vec3 bboxMax = glm::vec3(0.0f);
        
        Mesh() = default;
        Mesh(unsigned int vao, unsigned int vCount, unsigned int iCount = 0)
            : VAO(vao), vertexCount(vCount), indexCount(iCount) {}
            
        bool IsValid() const { return VAO != 0 && vertexCount > 0; }
    };

    // Composant Model complet
    struct Model : kComponent<Model> {
        std::vector<Mesh> meshes {};
        std::string filepath; // Chemin vers le fichier source (.glb, etc.)
        bool visible = true;
        bool castShadow = true;
        bool receiveShadow = true;
        int renderLayer = 0;
        
        // Bounding box du modèle complet
        glm::vec3 bboxMin = glm::vec3(0.0f);
        glm::vec3 bboxMax = glm::vec3(0.0f);
        
        // Animation
        bool animated = false;
        float animationTime = 0.0f;
        std::string currentAnimation;
        
        Model() = default;
        Model(const std::string& path) : filepath(path) {}
        
        // Méthodes utilitaires
        bool IsValid() const { return !meshes.empty(); }
        bool HasMeshes() const { return !meshes.empty(); }
        size_t GetMeshCount() const { return meshes.size(); }
        
        // Pour la compatibilité avec le code existant
        unsigned int GetVAO() const { return meshes.empty() ? 0 : meshes[0].VAO; }
        unsigned int GetVertexCount() const { return meshes.empty() ? 0 : meshes[0].vertexCount; }
        unsigned int GetIndexCount() const { return meshes.empty() ? 0 : meshes[0].indexCount; }
        
        // Gestion des matériaux
        void SetMaterialForAll(const Material& material) {
            for (auto& mesh : meshes) {
                mesh.material = material;
            }
        }
        
        void SetAlbedoForAll(const glm::vec3& albedo) {
            for (auto& mesh : meshes) {
                mesh.material.albedo = albedo;
            }
        }
        
        // Calcul de la bounding box
        void ComputeBoundingBox() {
            if (meshes.empty()) return;
            
            bboxMin = meshes[0].bboxMin;
            bboxMax = meshes[0].bboxMax;
            
            for (const auto& mesh : meshes) {
                bboxMin = glm::min(bboxMin, mesh.bboxMin);
                bboxMax = glm::max(bboxMax, mesh.bboxMax);
            }
        }
        
        // Vérification si un point est dans la bounding box
        bool IsPointInBoundingBox(const glm::vec3& point) const {
            return point.x >= bboxMin.x && point.x <= bboxMax.x &&
                   point.y >= bboxMin.y && point.y <= bboxMax.y &&
                   point.z >= bboxMin.z && point.z <= bboxMax.z;
        }
        
        // Obtention du centre de la bounding box
        glm::vec3 GetBoundingBoxCenter() const {
            return (bboxMin + bboxMax) * 0.5f;
        }
        
        // Obtention de la taille de la bounding box
        glm::vec3 GetBoundingBoxSize() const {
            return bboxMax - bboxMin;
        }
    };
}
