#include "Koji/ECS/Systems/RenderSystem.h"

#include <cstring>
#include <iostream>
#include <thread>

#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "../../../libs/bgfx.cmake/bgfx/src/bgfx_p.h"

// Multiplatform includes for GLFW
#if defined(_WIN32)
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>

#elif defined(__linux__)
    #define GLFW_EXPOSE_NATIVE_WAYLAND
    #include <GLFW/glfw3native.h>

#elif defined(__APPLE__)
    #define GLFW_EXPOSE_NATIVE_COCOA
    #include <GLFW/glfw3native.h>
#endif

#include "bx/math.h"
#include "Koji/Application.h"
#include "Koji/ResourceLoader.h"
#include "Koji/Scene.h"

// Simple shaders
static const char* vs_basic = R"(
    $input a_position, a_normal, a_texcoord0
    $output v_normal, v_texcoord0

    uniform mat4 u_modelMatrix;
    uniform mat4 u_viewMatrix;
    uniform mat4 u_projMatrix;

    void main() {
        vec4 worldPos = mul(u_modelMatrix, vec4(a_position, 1.0));
        vec4 viewPos = mul(u_viewMatrix, worldPos);
        gl_Position = mul(u_projMatrix, viewPos);
        
        v_normal = mul(u_modelMatrix, vec4(a_normal, 0.0)).xyz;
        v_texcoord0 = a_texcoord0;
    }
)";

static const char* fs_basic = R"(
    $input v_normal, v_texcoord0

    uniform sampler2D u_diffuseTexture;
    uniform vec3 u_lightDir;

    void main() {
        vec3 normal = normalize(v_normal);
        float ndotl = max(dot(normal, u_lightDir), 0.2);
        
        vec4 diffuse = texture2D(u_diffuseTexture, v_texcoord0);
        gl_FragColor = vec4(diffuse.rgb * ndotl, diffuse.a);
    }
)";

using namespace Koji::ECS;

RenderSystem::RenderSystem(uint32_t width, uint32_t height) 
    : width(width), height(height) {
}

bool RenderSystem::Init() {
    std::cout << "Initializing RenderSystem..." << std::endl;
    
    // GLFW
    if (!InitGLFW()) {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return false;
    }
    
    // BGFX
    if (!InitBGFX()) {
        std::cout << "Failed to initialize BGFX!" << std::endl;
        return false;
    }
    
    // shaders
    if (!InitShaders()) {
        std::cout << "Failed to initialize shaders!" << std::endl;
        return false;
    }
    
    // Camera
    InitCamera();
    
    // Uniforms
    u_modelMatrix = bgfx::createUniform("u_modelMatrix", bgfx::UniformType::Mat4);
    u_viewMatrix = bgfx::createUniform("u_viewMatrix", bgfx::UniformType::Mat4);
    u_projMatrix = bgfx::createUniform("u_projMatrix", bgfx::UniformType::Mat4);
    u_diffuseTexture = bgfx::createUniform("u_diffuseTexture", bgfx::UniformType::Sampler);
    u_lightDir = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
    
    if (!bgfx::isValid(u_modelMatrix) || !bgfx::isValid(u_viewMatrix) || 
        !bgfx::isValid(u_projMatrix) || !bgfx::isValid(u_diffuseTexture)) {
        std::cout << "Failed to create uniforms!" << std::endl;
        return false;
    }
    
    initialized = true;
    std::cout << "RenderSystem initialized successfully!" << std::endl;
    return true;
}

bool RenderSystem::InitGLFW() {
    // Configure error callback
    glfwSetErrorCallback(GLFWErrorCallback);

    
    // GLFW
    if (!glfwInit()) {
        std::cout << "GLFW init failed!" << std::endl;
        return false;
    }
    
    // Window config
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    // Create window
    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), "Koji Engine", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    
    // configure callbacks
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    
    std::cout << "GLFW initialized successfully!" << std::endl;
    return true;
}

bool RenderSystem::InitBGFX() {
    
    bgfx::Init init;
    init.type = bgfx::RendererType::Vulkan;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    
    bgfx::PlatformData pd{};
    
#if defined(__linux__)
    std::cout << "Setting up Wayland platform data..." << std::endl;
    pd.ndt = glfwGetWaylandDisplay();
    pd.nwh = (void*)(uintptr_t)glfwGetWaylandDisplay();
    std::cout << "Display: " << pd.ndt << ", window: " << pd.nwh << std::endl;
#elif defined(_WIN32)
    pd.nwh = glfwGetWin32Window(window);
#elif defined(__APPLE__)
    pd.nwh = glfwGetCocoaWindow(window);
#else
    std::cout << "Unsupported platform!" << std::endl;
    return false;
#endif
    
    init.platformData = pd;
    
    if (!bgfx::init(init)) {
        std::cout << "BGFX init failed!" << std::endl;
        return false;
    }
    
    // Configure view
    bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    bgfx::setViewRect(viewId, 0, 0, width, height);
    
    // Can enable debug (other options are avaible)
    // bgfx::setDebug(BGFX_DEBUG_TEXT);
    
    std::cout << "BGFX initialized successfully! Renderer: " 
              << bgfx::getRendererName(bgfx::getRendererType()) << std::endl;
    
    return true;
}

bool RenderSystem::InitShaders() {
    // Compile shaders in memory
    const bgfx::Memory* vsMem = bgfx::copy(vs_basic, strlen(vs_basic) + 1);
    const bgfx::Memory* fsMem = bgfx::copy(fs_basic, strlen(fs_basic) + 1);
    
    bgfx::ShaderHandle vsh = bgfx::createShader(vsMem);
    bgfx::ShaderHandle fsh = bgfx::createShader(fsMem);
    
    if (!bgfx::isValid(vsh) || !bgfx::isValid(fsh)) {
        std::cout << "Failed to create shaders!" << std::endl;
        return false;
    }
    
    // Create program
    defaultProgram = bgfx::createProgram(vsh, fsh, true);
    if (!bgfx::isValid(defaultProgram)) {
        std::cout << "Failed to create shader program!" << std::endl;
        return false;
    }
    
    // Give this program to the ResourceLoader
    auto& resourceLoader = ResourceLoader::GetInstance();
    // Note: Vous pourriez vouloir stocker le programme dans ResourceLoader
    // et l'assigner aux matériaux lors du chargement
    
    std::cout << "Shaders initialized successfully!" << std::endl;
    return true;
}

bool RenderSystem::InitCamera() {
    // View matrix
    bx::Vec3 eye = { 0.0f, 2.0f, -8.0f };
    bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
    bx::Vec3 up = { 0.0f, 1.0f, 0.0f };
    
    bx::mtxLookAt(viewMatrix, eye, at, up);
    
    // Projection matrix
    bx::mtxProj(projMatrix, 60.0f, float(width) / float(height), 0.1f, 100.0f, 
                bgfx::getCaps()->homogeneousDepth);
    
    return true;
}

bool RenderSystem::Update() {
    if (!initialized || glfwWindowShouldClose(window)) {
        return false;
    }
    
    // GLFW events polling
    glfwPollEvents();
    
    // Update camera
    UpdateCamera();
    
    // Start frame
    bgfx::touch(viewId);
    
    // Define view and projection matrices
    bgfx::setViewTransform(viewId, viewMatrix, projMatrix);
    bgfx::setUniform(u_viewMatrix, viewMatrix);
    bgfx::setUniform(u_projMatrix, projMatrix);
    
    // Light
    bx::Vec3 lightDirVec = bx::normalize(bx::Vec3{ 0.3f, -0.5f, -0.8f });
    float lightDir[4] = { lightDirVec.x, lightDirVec.y, lightDirVec.z, 0.0f };
    bgfx::setUniform(u_lightDir, lightDir);
    
    // Rendu de tous les modèles
    auto* world = Application::scene->GetWorld();
    if (world) {
        std::function<void(kTransform&, Model&)> callback = 
            [this](kTransform& transform, Model& model) {
                if (model.loaded && model.visible) {
                    RenderModel(model, transform);
                }
        };
        
        world->ForComponents<kTransform, Model>(callback);
    }
    
    // End frame
    bgfx::frame();
    
    return true;
}

void RenderSystem::RenderModel(const Model& model, const kTransform& transform) {
    // Model matrix
    float modelMatrix[16];
    bx::mtxIdentity(modelMatrix);
    
    // Translation
    bx::mtxTranslate(modelMatrix, transform.position.x, transform.position.y, transform.position.z);
    
    // Rotation (Euler)
    if (transform.rotation.x != 0.0f) {
        float rotX[16];
        bx::mtxRotateX(rotX, transform.rotation.x);
        bx::mtxMul(modelMatrix, modelMatrix, rotX);
    }
    if (transform.rotation.y != 0.0f) {
        float rotY[16];
        bx::mtxRotateY(rotY, transform.rotation.y);
        bx::mtxMul(modelMatrix, modelMatrix, rotY);
    }
    if (transform.rotation.z != 0.0f) {
        float rotZ[16];
        bx::mtxRotateZ(rotZ, transform.rotation.z);
        bx::mtxMul(modelMatrix, modelMatrix, rotZ);
    }
    
    // Scale
    bx::mtxScale(modelMatrix, transform.scale.x, transform.scale.y, transform.scale.z);
    
    // Define model matrices
    bgfx::setTransform(modelMatrix);
    bgfx::setUniform(u_modelMatrix, modelMatrix);
    
    // Render each mesh
    for (size_t i = 0; i < model.meshes.size(); i++) {
        const auto& mesh = model.meshes[i];
        
        // Define vertex/index buffer
        bgfx::setVertexBuffer(0, mesh.vertexBuffer);
        bgfx::setIndexBuffer(mesh.indexBuffer);
        
        // Define program
        bgfx::ProgramHandle program = defaultProgram;
        
        // use avaible material
        if (i < model.materials.size()) {
            const auto& material = model.materials[i];
            
            if (bgfx::isValid(material.program)) {
                program = material.program;
            }
            
            if (bgfx::isValid(material.diffuseTextureHandle)) {
                bgfx::setTexture(0, u_diffuseTexture, material.diffuseTextureHandle);
            }
        }
        
        // Submit draw call
        bgfx::submit(viewId, program);
    }
}

void RenderSystem::UpdateCamera() {
    // The camera don't move at this time
}

bool RenderSystem::Close() {
    std::cout << "Shutting down RenderSystem..." << std::endl;
    
    // Wait render calls to end
    bgfx::frame();
    bgfx::frame();
    
    // Destroy bgfx resources
    if (bgfx::isValid(defaultProgram)) {
        bgfx::destroy(defaultProgram);
    }
    if (bgfx::isValid(u_modelMatrix)) {
        bgfx::destroy(u_modelMatrix);
    }
    if (bgfx::isValid(u_viewMatrix)) {
        bgfx::destroy(u_viewMatrix);
    }
    if (bgfx::isValid(u_projMatrix)) {
        bgfx::destroy(u_projMatrix);
    }
    if (bgfx::isValid(u_diffuseTexture)) {
        bgfx::destroy(u_diffuseTexture);
    }
    if (bgfx::isValid(u_lightDir)) {
        bgfx::destroy(u_lightDir);
    }
    
    // Shutdown bgfx
    bgfx::shutdown();
    
    // Close GLFW
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
    
    initialized = false;
    std::cout << "RenderSystem shutdown completed!" << std::endl;
    return true;
}

void RenderSystem::GLFWErrorCallback(int error, const char* description) {
    std::cout << "GLFW Error (" << error << "): " << description << std::endl;
}

void RenderSystem::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // bgfxù view rect resize
    bgfx::reset(width, height, BGFX_RESET_VSYNC);
    bgfx::setViewRect(0, 0, 0, width, height);
    
    std::cout << "Window resized to: " << width << "x" << height << std::endl;
}