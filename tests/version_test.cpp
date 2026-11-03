#include <doctest/doctest.h>
#include <Koji/Version.h>

TEST_CASE("Build chain work") {
    CHECK(Koji::Version() != nullptr);
}