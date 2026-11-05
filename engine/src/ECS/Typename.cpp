#include <Koji/ECS/TypeName.h>

#if defined(__GNUG__)
#include <cxxabi.h>
#include <cstdlib>
#endif

namespace Koji::ECS
{
    std::string Demangle(const char* mangledName)
    {
#if defined(__GNUG__)
        int status = 0;
        // status == 0 when success
        char* demangled = abi::__cxa_demangle(mangledName, nullptr, nullptr, &status);
        std::string result = (status == 0 && demangled) ? demangled : mangledName;
        std::free(demangled);
        return result;
#else
        return mangledName;   // MSVC already give a demangled name
#endif
    }
}