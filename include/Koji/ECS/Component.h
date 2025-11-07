#pragma once
#include <string>
#include <typeinfo>

class ComponentRegistry {
public:
    struct Info {
        size_t size;
        size_t alignment;
        std::string name;
    };

    template<typename T>
    static size_t Register() {
        static const size_t id = counter++;
        registry()[id] = { sizeof(T), alignof(T), typeid(T).name() };
        return id;
    }

    static const Info& Get(size_t id) { return registry()[id]; }

private:
    static std::unordered_map<size_t, Info>& registry() {
        static std::unordered_map<size_t, Info> r;
        return r;
    }

    static inline size_t counter = 0;
};

struct kComponentBase {};

template<typename T>
struct kComponent : public kComponentBase {
    static const size_t TypeId; // Unique type ID

    static const char* GetName() {
        return ComponentRegistry::Get(TypeId).name.c_str();
    }
};

template<typename T>
const size_t kComponent<T>::TypeId = ComponentRegistry::Register<T>();
