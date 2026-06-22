#include "instrument.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Instrument returns error", "[Instrument]") {
    Instrument instrument;
    REQUIRE_THROWS_AS(instrument.payoff({}), std::logic_error);
}