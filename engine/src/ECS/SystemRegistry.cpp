#include <Koji/ECS/SystemRegistry.h>

namespace Koji::ECS
{
    SystemRegistry::~SystemRegistry()
    {
        // Destroy systems in the reverse order
        byType_.clear();
        while (!systems_.empty())
            systems_.pop_back();
    }

    System* SystemRegistry::Find(std::type_index type) const
    {
        const auto it = byType_.find(type);
        return it == byType_.end() ? nullptr : it->second;
    }
}