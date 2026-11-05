#include <Koji/ECS/System.h>
#include <Koji/ECS/TypeName.h>

namespace Koji::ECS
{
    std::string System::Name() const
    {
        // typeid(*this) donne le type réel (le plus dérivé) car System est polymorphe.
        return Demangle(typeid(*this).name());
    }
}