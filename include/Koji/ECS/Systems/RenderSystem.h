#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../System.h"
#include "../Components/Model.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <memory>

#include "Koji/ECS/Components/Transform.h"

class RenderSystem : public Koji::ECS::System {
private:
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    unsigned int shaderProgram;
    
    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;
    glm::vec3 cameraUp;
    
    // Debug
    bool wireframeMode;
    
public:
    RenderSystem(int width = 1920, int height = 1080, const std::string& title = "OpenGL 4.6");
    virtual ~RenderSystem();
    
    bool Init() override;
    bool Update() override;
    bool Close() override;
    
    void RenderModel(Koji::ECS::Transform& transform, Koji::ECS::Model& model);
    
    GLFWwindow* GetWindow() const { return window; }
    glm::mat4 GetProjectionMatrix() const { return projectionMatrix; }
    glm::mat4 GetViewMatrix() const { return viewMatrix; }
    unsigned int GetShaderProgram() const { return shaderProgram; }
    
    void SetCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
    void ToggleWireframe();
    void SetWireframe(bool enabled);
    
    // Advanced OpenGL 4.6 features
    void EnableDebugOutput();
    
private:
    bool InitGLFW();
    bool InitGLAD();
    bool CreateWindow();
    bool CompileShaders();
    void SetupMatrices();
    void PrintGPUInfo();
    
    unsigned int CompileShader(const std::string& source, GLenum type);
    unsigned int CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
    
    // Debug callback
    static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
                                          GLsizei length, const GLchar* message, const void* userParam);
};

#endif