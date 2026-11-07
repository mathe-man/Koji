#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <Koji/ECS/TypeName.h>
#include <Koji/ECS/Types.h>

namespace Koji::ECS
{
    struct ComponentInfo
    {
        ComponentId id = kInvalidComponent;
        std::string name;
        std::uint32_t size = 0;    // byte ; 0 for a tag (component without data)
        std::uint32_t align = 1;   // alignment

        bool IsTag() const { return size == 0; }
    };

    // Know every component types : name, size, alignment.
    // C++ components and components declared at runtime (C#, files) get into the same table
    class ComponentRegistry
    {
    public:
        // Register a component described at runtime: should be 'blittable' (Copyable with memcpy)
        // Same name and same layout -> should return the same existing component
        // Return kInvalidComponent (and fill LastError()) if the name is empty or already taken, if the alignment
        // is not a power of 2, if the size is not a multiple of the alignment,
        // if kMaxComponents is reached.
        ComponentId Register(const std::string& name, std::uint32_t size, std::uint32_t align);

        // Register a C++ type (size and alignment read using T).
        // Empty name => use T name. An empty struct (std::is_empty) give a tag of size 0.
        template <typename T>
        ComponentId RegisterType(const std::string& name = {})
        {
            static_assert(std::is_trivially_copyable_v<T>,
                          "a component must be trivially copyable (no std::string, no std::vector, no virtual): "
                          "it is moved with memcpy");

            const std::type_index key{typeid(T)};

            // Early return if the type is already registered
            if (const auto it = byType_.find(key); it != byType_.end())
                return it->second;

            // Get size and align of T
            constexpr std::uint32_t size  = std::is_empty_v<T> ? 0u : static_cast<std::uint32_t>(sizeof(T));
            constexpr std::uint32_t align = std::is_empty_v<T> ? 1u : static_cast<std::uint32_t>(alignof(T));

            const ComponentId id = Register(name.empty() ? Demangle(typeid(T).name()) : name, size, align);

            if (id != kInvalidComponent)
                byType_.emplace(key, id);
            return id;
        }

        // Id of an already registered C++ type, otherwise kInvalidComponent
        template <typename T>
        ComponentId IdOf() const
        {
            const auto it = byType_.find(std::type_index(typeid(T)));
            return it == byType_.end() ? kInvalidComponent : it->second;
        }

        // Id from a name (serialisation, C#), otherwise kInvalidComponent.
        ComponentId Find(const std::string& name) const;

        // nullptr the component is unknown. The pointer is valid until the next Register
        const ComponentInfo* Info(ComponentId id) const;

        std::size_t Count() const { return infos_.size(); }
        const std::string& LastError() const { return lastError_; }

    private:
        ComponentId Fail(std::string message);

        std::vector<ComponentInfo> infos_;  // Indexed by component id
        std::unordered_map<std::string, ComponentId> byName_;
        std::unordered_map<std::type_index, ComponentId> byType_;
        std::string lastError_;
    };
}