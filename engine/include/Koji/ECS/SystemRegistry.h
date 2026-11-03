#pragma once
#include <cstddef>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Koji/ECS/System.h>

namespace Koji::ECS
{
    // Own systems, only one system per type
    // Systems life cycle is managed by the SystemScheduler
    class SystemRegistry
    {
    public:
        SystemRegistry() = default;
        ~SystemRegistry();
        SystemRegistry(const SystemRegistry&) = delete;
        SystemRegistry& operator=(const SystemRegistry&) = delete;

        // Return the created system, nullptr if another system of this type already exist
        template <typename T, typename... Args>
        T* AddSystem(Args&&... args)
        {
            static_assert(std::is_base_of_v<System, T>, "T must derive from System");

            const std::type_index key{typeid(T)};
            if (byType_.contains(key))
                return nullptr;

            auto system = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = system.get();
            byType_.emplace(key, ptr);
            systems_.push_back(std::move(system));
            return ptr;
        }

        template <typename T>
        T* GetSystem() const
        {
            static_assert(std::is_base_of_v<System, T>, "T must derive from System");
            return static_cast<T*>(Find(std::type_index(typeid(T))));
        }

        template <typename T>
        bool HasSystem() const { return GetSystem<T>() != nullptr; }

        template <typename T>
        bool RemoveSystem()
        {
            static_assert(std::is_base_of_v<System, T>, "T must derive from System");

            const auto it = byType_.find(std::type_index(typeid(T)));
            if (it == byType_.end())
                return false;

            const System* target = it->second;
            byType_.erase(it);
            std::erase_if(systems_, [target](const std::unique_ptr<System>& s) { return s.get() == target; });
            return true;
        }

        System* Find(std::type_index type) const;

        const std::vector<std::unique_ptr<System>>& All() const { return systems_; }
        std::size_t Count() const { return systems_.size(); }

    private:
        std::vector<std::unique_ptr<System>> systems_;
        std::unordered_map<std::type_index, System*> byType_;
    };
}