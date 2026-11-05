#pragma once
#include <cstddef>
#include <string>
#include <vector>

#include <Koji/ECS/SystemContext.h>

namespace Koji::ECS
{
    // Resolve systems execution order and life time
    //
    // Usages
    //  - call Build() after modifying the register
    //  - Only modify (AddSystem(), RemoveSystem()) after calling Close()
    //  - The register need to stay alive after the scheduler
    class SystemScheduler
    {
    public:
        // Topological sort over SystemDesc::requiredSystems. A system always come after the systems he need
        // Original order is keep for independent systems
        // Return false and fill Error() if a required system is missing or an unsolvable pattern appear (circle referencing...)
        bool Build(SystemRegistry& registry);

        // Init each systems in order, if one system fail, already initiated systems are closed
        // in reverse order, then return false
        bool Init(SystemContext& ctx);

        // Update each system in order. Does nothing if Update() fail
        void Run(SystemContext& ctx);

        // Close in reverse order. Close every system even if one of them fail, but still return false
        bool Close(SystemContext& ctx);

        const std::vector<System*>& Order() const { return order_; }
        const std::string& Error() const { return error_; }
        bool IsInitialized() const { return initialized_; }

    private:
        // Close order_[count-1] ... order_[0]. Return false if at least one close failed.
        bool CloseFirst(SystemContext& ctx, std::size_t count);

        std::vector<System*> order_;
        SystemRegistry* registry_ = nullptr;
        std::string error_;
        bool initialized_ = false;
    };
}