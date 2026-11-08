#include <doctest/doctest.h>

#include <string>

#include <Koji/ECS/ComponentRegistry.h>

using namespace Koji::ECS;

namespace
{
    struct Position { float x, y, z; };
    struct Health { int value; };
    struct Flag {};   // Empty struct : give a tag of size 0
    // struct NotTrivial { std::string name; };  // RegisterType<NotTrivial>() ne compile pas : voulu

    bool Contains(const std::string& text, const char* part)
    {
        return text.find(part) != std::string::npos;
    }
}

TEST_CASE("Register give consecutive IDs and Info find them")
{
    ComponentRegistry reg;
    const ComponentId a = reg.Register("Position", 12, 4);
    const ComponentId b = reg.Register("Health", 4, 4);
    CHECK(a == 0u);
    CHECK(b == 1u);
    CHECK(reg.Count() == 2);

    const ComponentInfo* info = reg.Info(b);
    REQUIRE(info != nullptr);
    CHECK(info->id == b);
    CHECK(info->name == "Health");
    CHECK(info->size == 4u);
    CHECK(info->align == 4u);
    CHECK_FALSE(info->IsTag());
}

TEST_CASE("Registering the same thing multiple time give the same ID")
{
    ComponentRegistry reg;
    const ComponentId first = reg.Register("Health", 4, 4);
    const ComponentId again = reg.Register("Health", 4, 4);
    CHECK(first == again);
    CHECK(reg.Count() == 1);
}

TEST_CASE("Register refuse an already taken name with different layout")
{
    ComponentRegistry reg;
    reg.Register("Health", 4, 4);

    CHECK(reg.Register("Health", 8, 4) == kInvalidComponent);
    CHECK(Contains(reg.LastError(), "Health"));
    CHECK(Contains(reg.LastError(), "different layout"));
    CHECK(reg.Count() == 1);
}

TEST_CASE("Register refuse invalid descriptions")
{
    ComponentRegistry reg;

    CHECK(reg.Register("", 4, 4) == kInvalidComponent);
    CHECK(Contains(reg.LastError(), "empty"));

    CHECK(reg.Register("A", 4, 0) == kInvalidComponent);   // null alignment
    CHECK(reg.Register("B", 6, 3) == kInvalidComponent);   // alignment isn't a power of 2
    CHECK(Contains(reg.LastError(), "power of two"));

    CHECK(reg.Register("C", 6, 4) == kInvalidComponent);   // size and alignment are not multiple
    CHECK(Contains(reg.LastError(), "multiple"));

    CHECK(reg.Count() == 0);
}

TEST_CASE("Register accept a tag of size 0")
{
    ComponentRegistry reg;
    const ComponentId id = reg.Register("Player", 0, 1);
    REQUIRE(id != kInvalidComponent);
    CHECK(reg.Info(id)->IsTag());
}

TEST_CASE("Find retrieve a component by is name")
{
    ComponentRegistry reg;
    const ComponentId health = reg.Register("Health", 4, 4);
    CHECK(reg.Find("Health") == health);
    CHECK(reg.Find("Absent") == kInvalidComponent);
}

TEST_CASE("Info return nullptr for an unknown id")
{
    ComponentRegistry reg;
    reg.Register("Health", 4, 4);
    CHECK(reg.Info(1) == nullptr);
    CHECK(reg.Info(kInvalidComponent) == nullptr);
}

TEST_CASE("RegisterType read the size and align of a C++ type")
{
    ComponentRegistry reg;
    const ComponentId id = reg.RegisterType<Position>();
    REQUIRE(id != kInvalidComponent);

    const ComponentInfo* info = reg.Info(id);
    REQUIRE(info != nullptr);
    CHECK(info->size == sizeof(Position));
    CHECK(info->align == alignof(Position));
    CHECK(Contains(info->name, "Position"));   // Default readable name (demangled)
    CHECK(reg.Find(info->name) == id);
}

TEST_CASE("RegisterType allow explicit name")
{
    ComponentRegistry reg;
    const ComponentId id = reg.RegisterType<Position>("Pos");
    CHECK(reg.Find("Pos") == id);
}

TEST_CASE("RegisterType give the same id for an already registered type")
{
    ComponentRegistry reg;
    CHECK(reg.IdOf<Position>() == kInvalidComponent);   // Not registered yet

    const ComponentId first = reg.RegisterType<Position>();
    const ComponentId again = reg.RegisterType<Position>();
    CHECK(first == again);
    CHECK(reg.IdOf<Position>() == first);
    CHECK(reg.Count() == 1);

    const ComponentId health = reg.RegisterType<Health>();
    CHECK(health != first);
    CHECK(reg.IdOf<Health>() == health);
}

TEST_CASE("RegisterType transform an empty struct to a tag")
{
    ComponentRegistry reg;
    const ComponentId id = reg.RegisterType<Flag>();
    REQUIRE(id != kInvalidComponent);
    CHECK(reg.Info(id)->IsTag());
    CHECK(reg.Info(id)->size == 0u);
}

TEST_CASE("A C++ type cannot take an already registered type with a different layout")
{
    ComponentRegistry reg;
    reg.Register("Health", 8, 4);

    CHECK(reg.RegisterType<Health>("Health") == kInvalidComponent);
    CHECK(reg.IdOf<Health>() == kInvalidComponent);
}

TEST_CASE("The register don't get beyond kMaxComponents")
{
    ComponentRegistry reg;
    for (std::size_t i = 0; i < kMaxComponents; ++i)
        // std::to_string(unsigned long value) -> std::string
        REQUIRE(reg.Register("C" + std::to_string(i), 4, 4) != kInvalidComponent);

    CHECK(reg.Count() == kMaxComponents);
    CHECK(reg.Register("one too many", 4, 4) == kInvalidComponent);
    CHECK(Contains(reg.LastError(), "too many"));
    CHECK(reg.Count() == kMaxComponents);
}