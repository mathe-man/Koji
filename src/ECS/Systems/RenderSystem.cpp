#include "Koji/ECS/Systems/RenderSystem.h"
#include <iostream>
#include <fstream>

#include "glad/glad.h"
#include <GL/gl.h>
#include <GL/glext.h>


RenderSystem::RenderSystem(int width, int height, const std::string& title)
    : window(nullptr), windowTitle(title),
        width(width), height(height)
    {}

RenderSystem::~RenderSystem() {
    Close();
}


bool RenderSystem::Init() {
    std::cout << "Initializing RenderSystem with OpenGL 4.6..." << std::endl;
    
    if (!InitGLFW()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    if (!CreateWindow()) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }
    
    if (!InitGLAD()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    PrintGPUInfo();
    
    std::cout << "RenderSystem initialized successfully with OpenGL 4.6" << std::endl;
    
    return true;
}

bool RenderSystem::InitGLFW() {
    if (!glfwInit()) {
        return false;
    }
    
    return true;
}

bool RenderSystem::CreateWindow() {
    // Configuration pour OpenGL 4.6 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    // Création de la fenêtre
    window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window with OpenGL 4.6" << std::endl;
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync
    
    return true;
}

bool RenderSystem::InitGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    return true;
}

void RenderSystem::PrintGPUInfo() {
    std::cout << "=== OpenGL Information ===" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    // Informations supplémentaires
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    std::cout << "Max Texture Size: " << maxTextureSize << "x" << maxTextureSize << std::endl;
    
    GLint maxSamples;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    std::cout << "Max MSAA Samples: " << maxSamples << std::endl;
    
    // Vérifier les extensions importantes
    if (GL_ARB_direct_state_access) {
        std::cout << "DSA (Direct State Access): Supported" << std::endl;
    }
    if (GL_ARB_buffer_storage) {
        std::cout << "Persistent Mapped Buffers: Supported" << std::endl;
    }
    if (GL_ARB_multi_draw_indirect) {
        std::cout << "Multi Draw Indirect: Supported" << std::endl;
    }
    std::cout << "==========================" << std::endl;
}


bool RenderSystem::Update() {
    if (glfwWindowShouldClose(window)) {
        return false;
    }
    
    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
    
    return true;
}


bool RenderSystem::Close() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    
    glfwTerminate();
    std::cout << "RenderSystem closed" << std::endl;
    return true;
}
