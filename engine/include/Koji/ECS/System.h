#pragma once
#include <string>
#include <typeindex>
#include <typeinfo>
#include <vector>

#include <Koji/ECS/Types.h>

namespace Koji::ECS
{
    struct SystemContext;

    // System description, which other system does it requires, components it read/write
    // Useful for the Scheduler to sort the systems and parallelize then
    struct SystemDesc
    {
        std::vector<std::type_index> requiredSystems;
        std::vector<ComponentId> reads;
        std::vector<ComponentId> writes;

        // Allow chained writing by returning itself (*this)
        template <typename T>
        SystemDesc& Require()
        {
            requiredSystems.push_back(std::type_index(typeid(T)));
            return *this;
        }
        SystemDesc& Read(ComponentId id)  { reads.push_back(id);  return *this; }
        SystemDesc& Write(ComponentId id) { writes.push_back(id); return *this; }
    };

    // System base class
    class System
    {
    public:
        System() = default;
        virtual ~System() = default;
        System(const System&) = delete;
        System& operator=(const System&) = delete;

        [[nodiscard]] virtual SystemDesc Describe() const { return {}; }
        [[nodiscard]] std::string Name() const;

        virtual bool Init(SystemContext&)   { return true; }
        virtual void Update(SystemContext&) {}
        virtual bool Close(SystemContext&)  { return true; }
    };
}