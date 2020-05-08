#include <pineappl_capi.h>

#include <string>

#include <catch2/catch.hpp>

TEST_CASE("check storage creation", "[storage]")
{
    auto* f2 = pineappl_keyval_new();

    CHECK( f2 != nullptr );

    pineappl_keyval_delete(f2);
}

TEST_CASE("check f2 storage default values", "[storage]")
{
    auto* f2 = pineappl_keyval_new();

    CHECK( pineappl_keyval_int(f2, "nx") == 50);
    CHECK( pineappl_keyval_double(f2, "x_min") == 2e-7);
    CHECK( pineappl_keyval_double(f2, "x_max") == 1.0);
    CHECK( pineappl_keyval_int(f2, "x_order") == 3);
    CHECK( pineappl_keyval_int(f2, "nq2") == 30);
    CHECK( pineappl_keyval_double(f2, "q2_min") == 100);
    CHECK( pineappl_keyval_double(f2, "q2_max") == 1000000);
    CHECK( pineappl_keyval_int(f2, "q2_order") == 3);

    pineappl_keyval_delete(f2);
}

TEST_CASE("check storage getter and setter", "[storage]")
{
    auto* f2 = pineappl_keyval_new();

    pineappl_keyval_set_int(f2, "nx", 10);
    pineappl_keyval_set_int(f2, "x_order", 11);
    pineappl_keyval_set_int(f2, "nq2", 12);
    pineappl_keyval_set_int(f2, "q2_order", 13);

    CHECK( pineappl_keyval_int(f2, "nx") == 10 );
    CHECK( pineappl_keyval_int(f2, "x_order") == 11 );
    CHECK( pineappl_keyval_int(f2, "nq2") == 12 );
    CHECK( pineappl_keyval_int(f2, "q2_order") == 13 );

    pineappl_keyval_set_bool(f2, "reweight", true);

    CHECK( pineappl_keyval_bool(f2, "reweight") );

    pineappl_keyval_set_string(f2, "documentation", "foobar");

    CHECK( pineappl_keyval_string(f2, "documentation") == std::string("foobar") );

    pineappl_keyval_delete(f2);
}
