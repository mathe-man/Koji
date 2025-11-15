#pragma once
#include "../System.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class RenderSystem : public Koji::ECS::System {
private:
    GLFWwindow* window;
    int width, height; 
    std::string windowTitle;
    
    
public:
    RenderSystem(int width = 1920, int height = 1080, const std::string& title = "OpenGL 4.6");
    virtual ~RenderSystem();
    
    bool Init() override;
    bool Update() override;
    bool Close() override;
    
    
    [[nodiscard]] GLFWwindow* GetWindow() const { return window; }
    
private:
    bool InitGLFW();
    bool InitGLAD();
    bool CreateWindow();
    void PrintGPUInfo();
};

