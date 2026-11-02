#include <doctest/doctest.h>
#include <Koji/Version.h>

// doctest : TEST_CASE(name) déclare un test ; CHECK(expr) vérifie sans arrêter le test
TEST_CASE("la chaîne de build fonctionne") {
    CHECK(Koji::Version() != nullptr);
}