#include <Koji/ECS/SystemScheduler.h>
#include <Koji/ECS/TypeName.h>

#include <unordered_map>

namespace Koji::ECS
{
    bool SystemScheduler::Build(SystemRegistry& registry)
    {
        error_.clear();
        if (initialized_)
        {
            error_ = "Build() called while systems are initialized: call Close() first";
            return false;
        }
        order_.clear();
        registry_ = nullptr;

        const auto& systems = registry.All();
        const std::size_t n = systems.size();

        // Index of the systems in the order they were originally added
        std::unordered_map<const System*, std::size_t> indexOf;
        indexOf.reserve(n);
        for (std::size_t i = 0; i < n; ++i)
            indexOf.emplace(systems[i].get(), i);

        // pending[i]   : number of requirement for i not placed yet
        // dependents[d] : systèmes qui requièrent d
        std::vector<std::size_t> pending(n, 0);
        std::vector<std::vector<std::size_t>> dependents(n);

        for (std::size_t i = 0; i < n; ++i)
        {
            const SystemDesc desc = systems[i]->Describe();
            for (const std::type_index& type : desc.requiredSystems)
            {
                const System* dependency = registry.Find(type);
                if (!dependency)
                {
                    error_ = systems[i]->Name() + " requires " + Demangle(type.name())
                           + ", which is not registered";
                    return false;
                }
                dependents[indexOf.at(dependency)].push_back(i);
                ++pending[i];
            }
        }

        // Kahn algorythm : on each loop we place the systems for which every required system are already placed
        std::vector<bool> placed(n, false);
        std::vector<System*> result;
        result.reserve(n);

        while (result.size() < n)
        {
            std::size_t next = n;
            for (std::size_t i = 0; i < n; ++i)
            {
                if (!placed[i] && pending[i] == 0)
                {
                    next = i;
                    break;
                }
            }

            if (next == n)
            {
                // No more systems but it still have dependency
                error_ = "dependency cycle between systems: ";
                bool first = true;
                for (std::size_t i = 0; i < n; ++i)
                {
                    if (placed[i])
                        continue;
                    if (!first)
                        error_ += ", ";
                    error_ += systems[i]->Name();
                    first = false;
                }
                return false;
            }

            placed[next] = true;
            result.push_back(systems[next].get());
            for (std::size_t dependent : dependents[next])
                --pending[dependent];
        }

        order_ = std::move(result);
        registry_ = &registry;
        return true;
    }

    bool SystemScheduler::Init(SystemContext& ctx)
    {
        if (initialized_)
            return true;
        if (!registry_)
        {
            error_ = "Init() called before a successful Build()";
            return false;
        }

        error_.clear();
        ctx.systems = registry_;   // Systems can call ctx.GetSystem<T>() during Init

        for (std::size_t i = 0; i < order_.size(); ++i)
        {
            if (!order_[i]->Init(ctx))
            {
                error_ = "Init() failed for " + order_[i]->Name();
                CloseFirst(ctx, i);   // Only systems from 0..i-1 were inited
                return false;
            }
        }

        initialized_ = true;
        return true;
    }

    void SystemScheduler::Run(SystemContext& ctx)
    {
        if (!initialized_)
            return;

        ctx.systems = registry_;
        for (System* system : order_)
            system->Update(ctx);
    }

    bool SystemScheduler::Close(SystemContext& ctx)
    {
        if (!initialized_)
            return true;

        ctx.systems = registry_;
        initialized_ = false;
        return CloseFirst(ctx, order_.size());
    }

    bool SystemScheduler::CloseFirst(SystemContext& ctx, std::size_t count)
    {
        bool allOk = true;
        for (std::size_t k = count; k > 0; --k)
        {
            // Close() d'abord, pour qu'il soit toujours appelé même si un précédent a échoué.
            const bool ok = order_[k - 1]->Close(ctx);
            allOk = allOk && ok;
        }
        return allOk;
    }
}