#include "chooserEuropeanOption.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>

TEST_CASE("ChooserEuropeanOption returns error. invalid Maturity.",
          "[ChooserEuropeanOption]") {

    ChooserOptionParameters optionParamsCall{
        .strike = 10,
        .maturity = -1,
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(ChooserEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("ChooserEuropeanOption returns error. invalid OptionDirection.",
          "[ChooserEuropeanOption]") {

    ChooserOptionParameters optionParamsCall{
        .strike = 10,
        .maturity = 1,
        .direction = static_cast<OptionDirection>(99),
    };

    REQUIRE_THROWS_AS(ChooserEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("ChooserEuropeanOption returns error. Negative Strike.",
          "[ChooserEuropeanOption]") {

    ChooserOptionParameters optionParamsCall{
        .strike = -10,
        .maturity = 1,
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(ChooserEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("ChooserEuropeanOption returns correct.", "[ChooserEuropeanOption]") {

    ChooserOptionParameters optionParamsCall{
        .strike = 10,
        .maturity = 1,
        .direction = OptionDirection::Long,
    };

    ChooserEuropeanOption chooserOption =
        ChooserEuropeanOption{optionParamsCall};

    CHECK(chooserOption.parameters().direction == optionParamsCall.direction);
    CHECK(chooserOption.parameters().maturity == optionParamsCall.maturity);
    CHECK(chooserOption.parameters().strike == optionParamsCall.strike);
}