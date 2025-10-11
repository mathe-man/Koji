#include <Koji/ECS/Systems/RenderingSystem.h>
#include <Koji/ECS/Components/Transform.h>
#include <Koji/ECS/Components/Mesh.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // add translate() and scale()
#include <glm/gtx/quaternion.hpp>       // add mat4_cast()

using namespace Koji;

RenderingSystem::RenderingSystem(Renderer *renderer)
    : m_renderer(renderer)
{
}

void RenderingSystem::Render(entt::registry &registry) const {

    m_renderer->BeginFrame();

    auto view = registry.view<Transform, Mesh>();
    for (auto entity : view)
    {
        auto& transform = view.get<Transform>(entity);
        auto& mesh = view.get<Mesh>(entity);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position)
                        * glm::mat4_cast(transform.rotation)
                        * glm::scale(glm::mat4(1.0f), transform.scale);

        bgfx::setTransform(&model[0][0]);
        bgfx::setVertexBuffer(0, mesh.vbh);
        bgfx::setIndexBuffer(mesh.ibh);
        bgfx::submit(m_renderer->GetViewId(), mesh.program);
    }

    m_renderer->EndFrame();
}
