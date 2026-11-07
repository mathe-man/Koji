#include <Koji/ECS/ComponentRegistry.h>

namespace Koji::ECS
{
    ComponentId ComponentRegistry::Register(const std::string& name, std::uint32_t size, std::uint32_t align)
    {
        lastError_.clear();

        if (name.empty())
            return Fail("component name must not be empty");
        if (align == 0 || (align & (align - 1)) != 0)
            return Fail("component '" + name + "': alignment must be a power of two");
        if (size % align != 0)
            return Fail("component '" + name + "': size must be a multiple of its alignment");

        if (const auto it = byName_.find(name); it != byName_.end())
        {
            const ComponentInfo& existing = infos_[it->second];
            if (existing.size == size && existing.align == align)
                return existing.id;
            return Fail("component '" + name + "' is already registered with a different layout");
        }

        if (infos_.size() >= kMaxComponents)
                return Fail("too many components (maximum " + std::to_string(kMaxComponents) + ")");

        const ComponentId id = static_cast<ComponentId>(infos_.size());
        infos_.push_back(ComponentInfo{id, name, size, align});
        byName_.emplace(name, id);
        return id;
    }

    ComponentId ComponentRegistry::Find(const std::string& name) const
    {
        const auto it = byName_.find(name);
        return it == byName_.end() ? kInvalidComponent : it->second;
    }

    const ComponentInfo* ComponentRegistry::Info(ComponentId id) const
    {
        return id < infos_.size() ? &infos_[id] : nullptr;
    }

    ComponentId ComponentRegistry::Fail(std::string message)
    {
        lastError_ = std::move(message);
        return kInvalidComponent;
    }
}