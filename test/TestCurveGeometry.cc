#include "catch.hpp"

#include <ANurbs/Geometry/NurbsCurveGeometry.h>

using namespace ANurbs;


TEST_CASE( "Geometry of a spatial B-Spline curve", "[NurbsCurveGeometry]" ) {
    int degree = 4;
    int nb_poles = 8;
    bool is_rational = false;

    NurbsCurveGeometry<3> geometry(degree, nb_poles, is_rational);

    geometry.set_knot( 0,   0              );
    geometry.set_knot( 1,   0              );
    geometry.set_knot( 2,   0              );
    geometry.set_knot( 3,   0              );
    geometry.set_knot( 4,  32.9731425998736);
    geometry.set_knot( 5,  65.9462851997473);
    geometry.set_knot( 6,  98.9194277996209);
    geometry.set_knot( 7, 131.892570399495 );
    geometry.set_knot( 8, 131.892570399495 );
    geometry.set_knot( 9, 131.892570399495 );
    geometry.set_knot(10, 131.892570399495 );

    geometry.set_pole(0, {  0, -25, - 5});
    geometry.set_pole(1, {-15, -15,   0});
    geometry.set_pole(2, {  5, - 5, - 3});
    geometry.set_pole(3, { 15, -15,   3});
    geometry.set_pole(4, { 25,   0,   6});
    geometry.set_pole(5, { 15,  15,   6});
    geometry.set_pole(6, {- 5, - 5, - 3});
    geometry.set_pole(7, {-25,  15,   4});

    REQUIRE( geometry.dimension()  == 3     );
    REQUIRE( geometry.is_rational() == false );

    REQUIRE( geometry.degree()  ==  4 );
    REQUIRE( geometry.nb_knots() == 11 );
    REQUIRE( geometry.nb_poles() ==  8 );

    REQUIRE( geometry.domain().t0() ==   0              );
    REQUIRE( geometry.domain().t1() == 131.892570399495 );

    // SECTION( "Check Poles" ) {
    //     const auto poles = geometry.Poles();

    //     for (int i = 0; i < poles.size(); i++) {
    //         const auto actual = poles[i];
    //         const auto expected = geometry.Pole(i);
    //         CHECK( actual[0] == expected[0] );
    //         CHECK( actual[1] == expected[1] );
    //         CHECK( actual[2] == expected[2] );
    //     }
    // }

    // SECTION( "Check Weights" ) {
    //     const auto weights = geometry.Weights();

    //     for (int i = 0; i < weights.size(); i++) {
    //         const auto actual = weights[i];
    //         const auto expected = geometry.Weight(i);
    //         CHECK( actual == expected );
    //     }
    // }

    SECTION( "PointAt(t=0.0)" ) {
        auto point = geometry.point_at(0.0);

        CHECK( point[0] == Approx(  0) );
        CHECK( point[1] == Approx(-25) );
        CHECK( point[2] == Approx(- 5) );
    }

    SECTION( "PointAt(t=65.9462851997)" ) {
        auto point = geometry.point_at(65.9462851997);

        CHECK( point[0] == Approx( 18.8888888889) );
        CHECK( point[1] == Approx(- 6.1111111111) );
        CHECK( point[2] == Approx(  4.1666666667) );
    }

    SECTION( "PointAt(t=131.892570399495)" ) {
        auto point = geometry.point_at(131.892570399495);

        CHECK( point[0] == Approx(-25) );
        CHECK( point[1] == Approx( 15) );
        CHECK( point[2] == Approx(  4) );
    }

    SECTION( "DerivativesAt(t=0.0)" ) {
        auto derivatives = geometry.derivatives_at(0.0, 3);

        CHECK( derivatives[0][0] == Approx(  0) );
        CHECK( derivatives[0][1] == Approx(-25) );
        CHECK( derivatives[0][2] == Approx(- 5) );

        CHECK( derivatives[1][0] == Approx(- 1.81966277  ) );
        CHECK( derivatives[1][1] == Approx(  1.2131085134) );
        CHECK( derivatives[1][2] == Approx(  0.6065542567) );

        CHECK( derivatives[2][0] == Approx(  0.2759310497) );
        CHECK( derivatives[2][1] == Approx(- 0.0551862099) );
        CHECK( derivatives[2][2] == Approx(- 0.0717420729) );

        CHECK( derivatives[3][0] == Approx(- 0.0189682773) );
        CHECK( derivatives[3][1] == Approx(  0.0005578905) );
        CHECK( derivatives[3][2] == Approx(  0.005523116 ) );
    }

    SECTION( "DerivativesAt(t=65.9462851997)" ) {
        auto derivatives = geometry.derivatives_at(65.9462851997, 3);

        CHECK( derivatives[0][0] == Approx( 18.8888888889) );
        CHECK( derivatives[0][1] == Approx(- 6.1111111111) );
        CHECK( derivatives[0][2] == Approx(  4.1666666667) );

        CHECK( derivatives[1][0] == Approx(  0.2021847522) );
        CHECK( derivatives[1][1] == Approx(  0.336974587 ) );
        CHECK( derivatives[1][2] == Approx(  0.1010923761) );

        CHECK( derivatives[2][0] == Approx(- 0.0122636022) );
        CHECK( derivatives[2][1] == Approx(  0.0153295028) );
        CHECK( derivatives[2][2] == Approx(- 0.0036790807) );

        CHECK( derivatives[3][0] == Approx(- 0.0005578905) );
        CHECK( derivatives[3][1] == Approx(- 0.0006508723) );
        CHECK( derivatives[3][2] == Approx(  0.0000557891) );
    }

    SECTION( "DerivativesAt(t=131.892570399495)" ) {
        auto derivatives = geometry.derivatives_at(131.892570399495, 3);

        CHECK( derivatives[0][0] == Approx(-25           ) );
        CHECK( derivatives[0][1] == Approx( 15           ) );
        CHECK( derivatives[0][2] == Approx(  4           ) );

        CHECK( derivatives[1][0] == Approx(- 2.4262170267) );
        CHECK( derivatives[1][1] == Approx(  2.4262170267) );
        CHECK( derivatives[1][2] == Approx(  0.8491759593) );

        CHECK( derivatives[2][0] == Approx(- 0.1103724199) );
        CHECK( derivatives[2][1] == Approx(  0.3311172597) );
        CHECK( derivatives[2][2] == Approx(  0.1269282829) );

        CHECK( derivatives[3][0] == Approx(- 0.0044631241) );
        CHECK( derivatives[3][1] == Approx(  0.0251050729) );
        CHECK( derivatives[3][2] == Approx(  0.0092051934) );
    }
}

TEST_CASE( "Geometry of a spatial Nurbs curve", "[NurbsCurveGeometry]" ) {
    int degree = 4;
    int nb_poles = 8;
    bool isRational = true;

    NurbsCurveGeometry<3> geometry(degree, nb_poles, isRational);

    geometry.set_knot( 0,   0              );
    geometry.set_knot( 1,   0              );
    geometry.set_knot( 2,   0              );
    geometry.set_knot( 3,   0              );
    geometry.set_knot( 4,  32.9731425998736);
    geometry.set_knot( 5,  65.9462851997473);
    geometry.set_knot( 6,  98.9194277996209);
    geometry.set_knot( 7, 131.892570399495 );
    geometry.set_knot( 8, 131.892570399495 );
    geometry.set_knot( 9, 131.892570399495 );
    geometry.set_knot(10, 131.892570399495 );

    geometry.set_pole(0, {  0, -25, - 5});
    geometry.set_pole(1, {-15, -15,   0});
    geometry.set_pole(2, {  5, - 5, - 3});
    geometry.set_pole(3, { 15, -15,   3});
    geometry.set_pole(4, { 25,   0,   6});
    geometry.set_pole(5, { 15,  15,   6});
    geometry.set_pole(6, {- 5, - 5, - 3});
    geometry.set_pole(7, {-25,  15,   4});

    geometry.set_weight(0, 1.0);
    geometry.set_weight(1, 1.0);
    geometry.set_weight(2, 1.0);
    geometry.set_weight(3, 1.0);
    geometry.set_weight(4, 1.0);
    geometry.set_weight(5, 1.0);
    geometry.set_weight(6, 1.0);
    geometry.set_weight(7, 1.0);

    CHECK( geometry.dimension()  == 3     );
    CHECK( geometry.is_rational() == true  );

    CHECK( geometry.degree()  ==  4 );
    CHECK( geometry.nb_knots() == 11 );
    CHECK( geometry.nb_poles() ==  8 );

    CHECK( geometry.domain().t0() ==   0              );
    CHECK( geometry.domain().t1() == 131.892570399495 );

    SECTION( "PointAt(t=0.0)" ) {
        auto point = geometry.point_at(0.0);

        CHECK( point[0] == Approx(  0) );
        CHECK( point[1] == Approx(-25) );
        CHECK( point[2] == Approx(- 5) );
    }

    SECTION( "PointAt(t=65.9462851997)" ) {
        auto point = geometry.point_at(65.9462851997);

        REQUIRE( point[0] == Approx( 18.8888888889) );
        REQUIRE( point[1] == Approx(- 6.1111111111) );
        REQUIRE( point[2] == Approx(  4.1666666667) );
    }

    SECTION( "PointAt(t=131.892570399495)" ) {
        auto point = geometry.point_at(131.892570399495);

        REQUIRE( point[0] == Approx(-25) );
        REQUIRE( point[1] == Approx( 15) );
        REQUIRE( point[2] == Approx(  4) );
    }

    SECTION( "DerivativesAt(t=0.0)" ) {
        auto derivatives = geometry.derivatives_at(0.0, 3);

        REQUIRE( derivatives[0][0] == Approx(  0) );
        REQUIRE( derivatives[0][1] == Approx(-25) );
        REQUIRE( derivatives[0][2] == Approx(- 5) );

        REQUIRE( derivatives[1][0] == Approx(- 1.81966277  ) );
        REQUIRE( derivatives[1][1] == Approx(  1.2131085134) );
        REQUIRE( derivatives[1][2] == Approx(  0.6065542567) );

        REQUIRE( derivatives[2][0] == Approx(  0.2759310497) );
        REQUIRE( derivatives[2][1] == Approx(- 0.0551862099) );
        REQUIRE( derivatives[2][2] == Approx(- 0.0717420729) );

        REQUIRE( derivatives[3][0] == Approx(- 0.0189682773) );
        REQUIRE( derivatives[3][1] == Approx(  0.0005578905) );
        REQUIRE( derivatives[3][2] == Approx(  0.005523116 ) );
    }

    SECTION( "DerivativesAt(t=65.9462851997)" ) {
        auto derivatives = geometry.derivatives_at(65.9462851997, 3);

        REQUIRE( derivatives[0][0] == Approx( 18.8888888889) );
        REQUIRE( derivatives[0][1] == Approx(- 6.1111111111) );
        REQUIRE( derivatives[0][2] == Approx(  4.1666666667) );

        REQUIRE( derivatives[1][0] == Approx(  0.2021847522) );
        REQUIRE( derivatives[1][1] == Approx(  0.336974587 ) );
        REQUIRE( derivatives[1][2] == Approx(  0.1010923761) );

        REQUIRE( derivatives[2][0] == Approx(- 0.0122636022) );
        REQUIRE( derivatives[2][1] == Approx(  0.0153295028) );
        REQUIRE( derivatives[2][2] == Approx(- 0.0036790807) );

        REQUIRE( derivatives[3][0] == Approx(- 0.0005578905) );
        REQUIRE( derivatives[3][1] == Approx(- 0.0006508723) );
        REQUIRE( derivatives[3][2] == Approx(  0.0000557891) );
    }

    SECTION( "DerivativesAt(t=131.892570399495)" ) {
        auto derivatives = geometry.derivatives_at(131.892570399495, 3);

        REQUIRE( derivatives[0][0] == Approx(-25           ) );
        REQUIRE( derivatives[0][1] == Approx( 15           ) );
        REQUIRE( derivatives[0][2] == Approx(  4           ) );

        REQUIRE( derivatives[1][0] == Approx(- 2.4262170267) );
        REQUIRE( derivatives[1][1] == Approx(  2.4262170267) );
        REQUIRE( derivatives[1][2] == Approx(  0.8491759593) );

        REQUIRE( derivatives[2][0] == Approx(- 0.1103724199) );
        REQUIRE( derivatives[2][1] == Approx(  0.3311172597) );
        REQUIRE( derivatives[2][2] == Approx(  0.1269282829) );

        REQUIRE( derivatives[3][0] == Approx(- 0.0044631241) );
        REQUIRE( derivatives[3][1] == Approx(  0.0251050729) );
        REQUIRE( derivatives[3][2] == Approx(  0.0092051934) );
    }
}

// TEST_CASE( "Refinement of a BSpline curve", "[NurbsCurveGeometry][Refinement][BSpline]" ) {
//     NurbsCurveGeometry<1> curve(2, 4, false);

//     curve.set_knot(0, 0.0);
//     curve.set_knot(1, 0.0);
//     curve.set_knot(2, 1.0);
//     curve.set_knot(3, 2.0);
//     curve.set_knot(4, 2.0);

//     curve.set_pole(0, {0.00});
//     curve.set_pole(1, {1.25});
//     curve.set_pole(2, {3.75});
//     curve.set_pole(3, {5.00});

//     std::vector<double> newKnots = {0.4, 0.8, 1.2};

//     // auto result = *curve.Refined(newKnots);

//     // SECTION( "Check knots" ) {
//     //     CHECK( result.nb_knots() == 8 );

//     //     CHECK( result.Knot(0) == Approx( 0.0 ) );
//     //     CHECK( result.Knot(1) == Approx( 0.0 ) );
//     //     CHECK( result.Knot(2) == Approx( 0.4 ) );
//     //     CHECK( result.Knot(3) == Approx( 0.8 ) );
//     //     CHECK( result.Knot(4) == Approx( 1.0 ) );
//     //     CHECK( result.Knot(5) == Approx( 1.2 ) );
//     //     CHECK( result.Knot(6) == Approx( 2.0 ) );
//     //     CHECK( result.Knot(7) == Approx( 2.0 ) );
//     // }

//     // SECTION( "Check poles" ) {
//     //     CHECK( result.nb_poles() == 7 );

//     //     CHECK( result.Pole(0).X() == Approx( 0.00 ) );
//     //     CHECK( result.Pole(1).X() == Approx( 0.50 ) );
//     //     CHECK( result.Pole(2).X() == Approx( 1.50 ) );
//     //     CHECK( result.Pole(3).X() == Approx( 2.25 ) );
//     //     CHECK( result.Pole(4).X() == Approx( 2.75 ) );
//     //     CHECK( result.Pole(5).X() == Approx( 4.00 ) );
//     //     CHECK( result.Pole(6).X() == Approx( 5.00 ) );
//     // }
// }

// TEST_CASE( "Refinement of a Nurbs curve", "[NurbsCurveGeometry][Refinement][Nurbs]" ) {
//     NurbsCurveGeometry<1> curve(2, 4, true);

//     curve.set_knot(0, 0.0);
//     curve.set_knot(1, 0.0);
//     curve.set_knot(2, 1.0);
//     curve.set_knot(3, 2.0);
//     curve.set_knot(4, 2.0);

//     curve.set_pole(0, {0.00});
//     curve.set_pole(1, {1.25});
//     curve.set_pole(2, {3.75});
//     curve.set_pole(3, {5.00});

//     curve.set_weight(0, 1.0);
//     curve.set_weight(1, 5.0);
//     curve.set_weight(2, 1.0);
//     curve.set_weight(3, 1.0);

//     std::vector<double> newKnots = {0.4, 0.8, 1.2};

//     // auto result = *curve.Refined(newKnots);

//     // SECTION( "Check knots" ) {
//     //     CHECK( result.nb_knots() == 8 );

//     //     CHECK( result.Knot(0) == Approx( 0.0 ) );
//     //     CHECK( result.Knot(1) == Approx( 0.0 ) );
//     //     CHECK( result.Knot(2) == Approx( 0.4 ) );
//     //     CHECK( result.Knot(3) == Approx( 0.8 ) );
//     //     CHECK( result.Knot(4) == Approx( 1.0 ) );
//     //     CHECK( result.Knot(5) == Approx( 1.2 ) );
//     //     CHECK( result.Knot(6) == Approx( 2.0 ) );
//     //     CHECK( result.Knot(7) == Approx( 2.0 ) );
//     // }

//     // SECTION( "Check weights" ) {
//     //     CHECK( result.nb_poles() == 7 );

//     //     CHECK( result.Weight(0) == Approx( 1.00 ) );
//     //     CHECK( result.Weight(1) == Approx( 2.60 ) );
//     //     CHECK( result.Weight(2) == Approx( 3.88 ) );
//     //     CHECK( result.Weight(3) == Approx( 3.40 ) );
//     //     CHECK( result.Weight(4) == Approx( 2.60 ) );
//     //     CHECK( result.Weight(5) == Approx( 1.00 ) );
//     //     CHECK( result.Weight(6) == Approx( 1.00 ) );
//     // }

//     // SECTION( "Check poles" ) {
//     //     CHECK( result.nb_poles() == 7 );

//     //     CHECK( result.Pole(0).X() == Approx( 0.0000000000 ) );
//     //     CHECK( result.Pole(1).X() == Approx( 0.9615384615 ) );
//     //     CHECK( result.Pole(2).X() == Approx( 1.3144329897 ) );
//     //     CHECK( result.Pole(3).X() == Approx( 1.5441176471 ) );
//     //     CHECK( result.Pole(4).X() == Approx( 1.8269230769 ) );
//     //     CHECK( result.Pole(5).X() == Approx( 4.0000000000 ) );
//     //     CHECK( result.Pole(6).X() == Approx( 5.0000000000 ) );
//     // }
// }
