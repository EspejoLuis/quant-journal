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

TEST_CASE("ChooserEuropeanOption long/short returns correct.",
          "[ChooserEuropeanOption]") {

    ChooserOptionParameters optionParamsCallLong{
        .strike = 10,
        .maturity = 1,
        .direction = OptionDirection::Long,
    };

    ChooserEuropeanOption chooserOptionLong =
        ChooserEuropeanOption{optionParamsCallLong};

    CHECK(chooserOptionLong.parameters().direction ==
          optionParamsCallLong.direction);
    CHECK(chooserOptionLong.parameters().maturity ==
          optionParamsCallLong.maturity);
    CHECK(chooserOptionLong.parameters().strike == optionParamsCallLong.strike);

    ChooserOptionParameters optionParamsCallShort{
        .strike = 10, .maturity = 1, .direction = OptionDirection::Short};

    ChooserEuropeanOption chooserOptionShort =
        ChooserEuropeanOption{optionParamsCallShort};

    CHECK(chooserOptionShort.parameters().direction ==
          optionParamsCallShort.direction);
    CHECK(chooserOptionShort.parameters().maturity ==
          optionParamsCallShort.maturity);
    CHECK(chooserOptionShort.parameters().strike ==
          optionParamsCallShort.strike);
}