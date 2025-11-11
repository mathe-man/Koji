#include "Koji/ECS/Systems/RenderSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glext.h>

#include "Koji/Application.h"
#include "Koji/Scene.h"
#include "Koji/ECS/Systems/Time.h"

// Shaders modernes OpenGL 4.6
const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    // PBR-inspired lighting (simplifié)
    float ambientStrength = 0.02;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular avec roughness
    float roughness = 0.5;
    float specularStrength = 0.5 * (1.0 - roughness);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0 / roughness);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Tonemapping simple
    vec3 result = (ambient + diffuse + specular) * objectColor;
    result = result / (result + vec3(1.0)); // Reinhard tonemapping
    
    FragColor = vec4(result, 1.0);
}
)";

RenderSystem::RenderSystem(int width, int height, const std::string& title)
    : window(nullptr), windowWidth(width), windowHeight(height), 
      windowTitle(title), shaderProgram(0),
      cameraPosition(0.0f, 0.0f, 5.0f),
      cameraTarget(0.0f, 0.0f, 0.0f),
      cameraUp(0.0f, 1.0f, 0.0f),
      wireframeMode(false) {
}

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
    
    // Enable debug output (OpenGL 4.3+)
    if (GLAD_GL_VERSION_4_3) {
        EnableDebugOutput();
    }
    
    if (!CompileShaders()) {
        std::cerr << "Failed to compile shaders" << std::endl;
        return false;
    }
    
    SetupMatrices();
    
    // Configuration OpenGL moderne
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Multisampling si supporté
    glEnable(GL_MULTISAMPLE);
    
    // Blend moderne
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window with OpenGL 4.6" << std::endl;
        
        // Fallback to OpenGL 4.5
        std::cout << "Trying OpenGL 4.5..." << std::endl;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);
        
        if (!window) {
            // Fallback to OpenGL 4.3
            std::cout << "Trying OpenGL 4.3..." << std::endl;
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);
            
            if (!window) {
                glfwTerminate();
                return false;
            }
        }
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

void RenderSystem::EnableDebugOutput() {
    if (GLAD_GL_VERSION_4_3 || GL_KHR_debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        std::cout << "OpenGL Debug Output enabled" << std::endl;
    }
}

void GLAPIENTRY RenderSystem::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
                                             GLsizei length, const GLchar* message, const void* userParam) {
    // Ignorer les messages non importants
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
    
    std::cerr << "OpenGL Debug: ";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cerr << "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cerr << "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cerr << "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cerr << "PERFORMANCE";
            break;
        default:
            std::cerr << "OTHER";
            break;
    }
    
    std::cerr << " - " << message << std::endl;
}

bool RenderSystem::CompileShaders() {
    shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    return shaderProgram != 0;
}

unsigned int RenderSystem::CompileShader(const std::string& source, GLenum type) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

unsigned int RenderSystem::CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);
    
    if (vs == 0 || fs == 0) {
        return 0;
    }
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(program);
        program = 0;
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;
}

void RenderSystem::SetupMatrices() {
    projectionMatrix = glm::perspective(glm::radians(60.0f), 
                                       (float)windowWidth / (float)windowHeight, 
                                       0.1f, 1000.0f);
    
    viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
}

bool RenderSystem::Update() {
    if (glfwWindowShouldClose(window)) {
        return false;
    }
    
    // Clear screen avec une couleur moderne
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(shaderProgram);
    
    // Set shader uniforms
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projectionMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 5.0f, 5.0f, 5.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
    
    Koji::Application::scene->GetWorld()->ForComponents<Koji::ECS::Transform, Koji::ECS::Model>([this](Koji::ECS::Transform& transform, Koji::ECS::Model& model)
        {
            RenderModel(transform, model);   
        }
    );
    
    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
    
    return true;
}

void RenderSystem::RenderModel(Koji::ECS::Transform& transform, Koji::ECS::Model& model)
{
    std::cout << "model pointer: " << &model << ", meshes size: " << model.meshes.size() << std::endl;

    if (model.visible && model.IsValid()) {
        glm::mat4 modelMatrix = transform.GetModelMatrix();
            
        // Rendu de chaque mesh du modèle
        for (const auto& mesh : model.meshes) {
            if (mesh.IsValid()) {
                // Matrice model
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &modelMatrix[0][0]);
                    
                // Propriétés du matériau
                glUniform3fv(glGetUniformLocation(shaderProgram, "material.albedo"), 1, &mesh.material.albedo[0]);
                glUniform1f(glGetUniformLocation(shaderProgram, "material.metallic"), mesh.material.metallic);
                glUniform1f(glGetUniformLocation(shaderProgram, "material.roughness"), mesh.material.roughness);
                glUniform1f(glGetUniformLocation(shaderProgram, "material.ao"), mesh.material.ao);
                glUniform1f(glGetUniformLocation(shaderProgram, "material.alpha"), mesh.material.alpha);
                    
                // Rendu du mesh
                glBindVertexArray(mesh.VAO);
                if (mesh.indexCount > 0) {
                    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
                } else {
                    glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
                }
                glBindVertexArray(0);
            }
        }
    }
}

bool RenderSystem::Close() {
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
    
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    
    glfwTerminate();
    std::cout << "RenderSystem closed" << std::endl;
    return true;
}

void RenderSystem::SetCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
    cameraPosition = position;
    cameraTarget = target;
    cameraUp = up;
    viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
}

void RenderSystem::ToggleWireframe() {
    wireframeMode = !wireframeMode;
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void RenderSystem::SetWireframe(bool enabled) {
    wireframeMode = enabled;
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}