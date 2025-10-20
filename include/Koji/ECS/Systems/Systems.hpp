#include "entt/entity/registry.hpp"


namespace Koji::Systems
{
    class System
    {
    public:
        System();
        virtual ~System() = default;
        
        virtual bool Frame(entt::registry registry)         = 0;
        virtual bool BeginFrame(entt::registry registry)    = 0;
        virtual bool EndFrame(entt::registry registry)      = 0;
    };

// Add existing systems to namesapce
#include "RenderingSystem.hpp"

}
