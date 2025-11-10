#pragma once
#include "../System.h"
#include "../Components/Transform.h"
#include "../Components/Model.h"
#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>

namespace Koji::ECS
{
    class RenderSystem : public System {
    private:
        // Window
        GLFWwindow* window = nullptr;
        uint32_t width = 1280;
        uint32_t height = 720;
        
        // bgfx uniforms 
        bgfx::UniformHandle u_modelMatrix;
        bgfx::UniformHandle u_viewMatrix;
        bgfx::UniformHandle u_projMatrix;
        bgfx::UniformHandle u_diffuseTexture;
        bgfx::UniformHandle u_lightDir;
        
        // shaders handle 
        bgfx::ProgramHandle defaultProgram;
        
        // camera matrix
        float viewMatrix[16];
        float projMatrix[16];
        
        // View ID
        bgfx::ViewId viewId = 0;
        
        // state
        bool initialized = false;

    public:
        RenderSystem(uint32_t width = 1280, uint32_t height = 720);
        ~RenderSystem() override = default;
        
        bool Init() override;
        bool Update() override;
        bool Close() override;
        
        // Getters
        GLFWwindow* GetWindow() const { return window; }
        bool IsInitialized() const { return initialized; }

    private:
        // Init
        bool InitGLFW();
        bool InitBGFX();
        bool InitShaders();
        bool InitCamera();
        
        
        void RenderModel(const Model& model, const kTransform& transform);
        void UpdateCamera();
        
        // Callbacks
        static void GLFWErrorCallback(int error, const char* description);
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    };
}
