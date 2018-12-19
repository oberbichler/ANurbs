#include "catch.hpp"

#include <ANurbs/src/Grid.h>
#include <ANurbs/src/SurfaceShapeEvaluator.h>

TEST_CASE( "Shape functions of a B-Spline surface",
    "[SurfaceShapeEvaluator]" )
{
    using namespace ANurbs;

    const int degreeU = 2;
    const int degreeV = 1;
    const int order = 2;

    SurfaceShapeEvaluator<double> shape(degreeU, degreeV, order);

    std::vector<double> knotsU = {0, 0, 7.5, 15, 15.0};
    std::vector<double> knotsV = {0, 10, 20};

    const double u = 12;
    const double v = 5;

    shape.Compute(knotsU, knotsV, u, v);

    CHECK( shape.DegreeU() == 2 );
    CHECK( shape.DegreeV() == 1 );
    CHECK( shape.NbShapes() == 6 );
    CHECK( shape.NbNonzeroPoles() == 6 );
    CHECK( shape.NbNonzeroPolesU() == 3 );
    CHECK( shape.NbNonzeroPolesV() == 2 );
    CHECK( shape.FirstNonzeroPoleU() == 1 );
    CHECK( shape.FirstNonzeroPoleV() == 0 );
    CHECK( shape.LastNonzeroPoleU() == 3 );
    CHECK( shape.LastNonzeroPoleV() == 1 );

    SECTION( "Check nonzero pole indices" ) {
        const auto nonZeroPoleIndices = shape.NonzeroPoleIndices();

        CHECK( nonZeroPoleIndices.size() == 6 );

        CHECK( nonZeroPoleIndices[0] == std::pair<int, int>(1, 0) );
        CHECK( nonZeroPoleIndices[1] == std::pair<int, int>(1, 1) );
        CHECK( nonZeroPoleIndices[2] == std::pair<int, int>(2, 0) );
        CHECK( nonZeroPoleIndices[3] == std::pair<int, int>(2, 1) );
        CHECK( nonZeroPoleIndices[4] == std::pair<int, int>(3, 0) );
        CHECK( nonZeroPoleIndices[5] == std::pair<int, int>(3, 1) );
    }

    SECTION( "Double index access" ) {
        REQUIRE( shape(0, 0, 0) == Approx( 0.04        ) );
        REQUIRE( shape(0, 0, 1) == Approx( 0.04        ) );
        REQUIRE( shape(0, 1, 0) == Approx( 0.28        ) );
        REQUIRE( shape(0, 1, 1) == Approx( 0.28        ) );
        REQUIRE( shape(0, 2, 0) == Approx( 0.18        ) );
        REQUIRE( shape(0, 2, 1) == Approx( 0.18        ) );

        REQUIRE( shape(1, 0, 0) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 0, 1) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 1, 0) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 1, 1) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 2, 0) == Approx( 0.08        ) );
        REQUIRE( shape(1, 2, 1) == Approx( 0.08        ) );

        // check second time

        shape.Compute(knotsU, knotsV, u, v);

        REQUIRE( shape(0, 0, 0) == Approx( 0.04        ) );
        REQUIRE( shape(0, 0, 1) == Approx( 0.04        ) );
        REQUIRE( shape(0, 1, 0) == Approx( 0.28        ) );
        REQUIRE( shape(0, 1, 1) == Approx( 0.28        ) );
        REQUIRE( shape(0, 2, 0) == Approx( 0.18        ) );
        REQUIRE( shape(0, 2, 1) == Approx( 0.18        ) );

        REQUIRE( shape(1, 0, 0) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 0, 1) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 1, 0) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 1, 1) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 2, 0) == Approx( 0.08        ) );
        REQUIRE( shape(1, 2, 1) == Approx( 0.08        ) );
    }

    SECTION( "Single index access" ) {
        REQUIRE( shape(0, 0) == Approx( 0.04        ) );
        REQUIRE( shape(0, 1) == Approx( 0.04        ) );
        REQUIRE( shape(0, 2) == Approx( 0.28        ) );
        REQUIRE( shape(0, 3) == Approx( 0.28        ) );
        REQUIRE( shape(0, 4) == Approx( 0.18        ) );
        REQUIRE( shape(0, 5) == Approx( 0.18        ) );

        REQUIRE( shape(1, 0) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 1) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 2) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 3) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 4) == Approx( 0.08        ) );
        REQUIRE( shape(1, 5) == Approx( 0.08        ) );

        // check second time

        shape.Compute(knotsU, knotsV, u, v);

        REQUIRE( shape(0, 0) == Approx( 0.04        ) );
        REQUIRE( shape(0, 1) == Approx( 0.04        ) );
        REQUIRE( shape(0, 2) == Approx( 0.28        ) );
        REQUIRE( shape(0, 3) == Approx( 0.28        ) );
        REQUIRE( shape(0, 4) == Approx( 0.18        ) );
        REQUIRE( shape(0, 5) == Approx( 0.18        ) );

        REQUIRE( shape(1, 0) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 1) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 2) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 3) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 4) == Approx( 0.08        ) );
        REQUIRE( shape(1, 5) == Approx( 0.08        ) );
    }
}

TEST_CASE( "Shape functions of a equal weighted Nurbs surface",
    "[SurfaceShapeEvaluator]" )
{
    using namespace ANurbs;

    const int degreeU = 2;
    const int degreeV = 1;
    const int order = 2;

    SurfaceShapeEvaluator<double> shape(degreeU, degreeV, order);
 
    std::vector<double> knotsU = {0, 0, 7.5, 15, 15.0};
    std::vector<double> knotsV = {0, 10, 20};

    Grid<double> weights(4, 3);
    weights.SetValue(0, 0, 2.0);
    weights.SetValue(0, 1, 2.0);
    weights.SetValue(0, 2, 2.0);
    weights.SetValue(1, 0, 2.0);
    weights.SetValue(1, 1, 2.0);
    weights.SetValue(1, 2, 2.0);
    weights.SetValue(2, 0, 2.0);
    weights.SetValue(2, 1, 2.0);
    weights.SetValue(2, 2, 2.0);
    weights.SetValue(3, 0, 2.0);
    weights.SetValue(3, 1, 2.0);
    weights.SetValue(3, 2, 2.0);

    const double u = 12;
    const double v = 5;

    shape.Compute(knotsU, knotsV, weights, u, v);

    CHECK( shape.DegreeU() == 2 );
    CHECK( shape.DegreeV() == 1 );
    CHECK( shape.NbShapes() == 6 );
    CHECK( shape.NbNonzeroPoles() == 6 );
    CHECK( shape.NbNonzeroPolesU() == 3 );
    CHECK( shape.NbNonzeroPolesV() == 2 );
    CHECK( shape.FirstNonzeroPoleU() == 1 );
    CHECK( shape.FirstNonzeroPoleV() == 0 );
    CHECK( shape.LastNonzeroPoleU() == 3 );
    CHECK( shape.LastNonzeroPoleV() == 1 );

    SECTION( "Check nonzero pole indices" ) {
        const auto nonZeroPoleIndices = shape.NonzeroPoleIndices();

        CHECK( nonZeroPoleIndices.size() == 6 );

        CHECK( nonZeroPoleIndices[0] == std::pair<int, int>(1, 0) );
        CHECK( nonZeroPoleIndices[1] == std::pair<int, int>(1, 1) );
        CHECK( nonZeroPoleIndices[2] == std::pair<int, int>(2, 0) );
        CHECK( nonZeroPoleIndices[3] == std::pair<int, int>(2, 1) );
        CHECK( nonZeroPoleIndices[4] == std::pair<int, int>(3, 0) );
        CHECK( nonZeroPoleIndices[5] == std::pair<int, int>(3, 1) );
    }

    SECTION( "Double index access" ) {
        REQUIRE( shape(0, 0, 0) == Approx( 0.04        ) );
        REQUIRE( shape(0, 0, 1) == Approx( 0.04        ) );
        REQUIRE( shape(0, 1, 0) == Approx( 0.28        ) );
        REQUIRE( shape(0, 1, 1) == Approx( 0.28        ) );
        REQUIRE( shape(0, 2, 0) == Approx( 0.18        ) );
        REQUIRE( shape(0, 2, 1) == Approx( 0.18        ) );

        REQUIRE( shape(1, 0, 0) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 0, 1) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 1, 0) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 1, 1) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 2, 0) == Approx( 0.08        ) );
        REQUIRE( shape(1, 2, 1) == Approx( 0.08        ) );

        // check second time

        shape.Compute(knotsU, knotsV, weights, u, v);

        REQUIRE( shape(0, 0, 0) == Approx( 0.04        ) );
        REQUIRE( shape(0, 0, 1) == Approx( 0.04        ) );
        REQUIRE( shape(0, 1, 0) == Approx( 0.28        ) );
        REQUIRE( shape(0, 1, 1) == Approx( 0.28        ) );
        REQUIRE( shape(0, 2, 0) == Approx( 0.18        ) );
        REQUIRE( shape(0, 2, 1) == Approx( 0.18        ) );

        REQUIRE( shape(1, 0, 0) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 0, 1) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 1, 0) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 1, 1) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 2, 0) == Approx( 0.08        ) );
        REQUIRE( shape(1, 2, 1) == Approx( 0.08        ) );
    }

    SECTION( "Single index access" ) {
        REQUIRE( shape(0, 0) == Approx( 0.04        ) );
        REQUIRE( shape(0, 1) == Approx( 0.04        ) );
        REQUIRE( shape(0, 2) == Approx( 0.28        ) );
        REQUIRE( shape(0, 3) == Approx( 0.28        ) );
        REQUIRE( shape(0, 4) == Approx( 0.18        ) );
        REQUIRE( shape(0, 5) == Approx( 0.18        ) );

        REQUIRE( shape(1, 0) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 1) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 2) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 3) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 4) == Approx( 0.08        ) );
        REQUIRE( shape(1, 5) == Approx( 0.08        ) );

        // check second time

        shape.Compute(knotsU, knotsV, weights, u, v);

        REQUIRE( shape(0, 0) == Approx( 0.04        ) );
        REQUIRE( shape(0, 1) == Approx( 0.04        ) );
        REQUIRE( shape(0, 2) == Approx( 0.28        ) );
        REQUIRE( shape(0, 3) == Approx( 0.28        ) );
        REQUIRE( shape(0, 4) == Approx( 0.18        ) );
        REQUIRE( shape(0, 5) == Approx( 0.18        ) );

        REQUIRE( shape(1, 0) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 1) == Approx(-0.0266666667) );
        REQUIRE( shape(1, 2) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 3) == Approx(-0.0533333333) );
        REQUIRE( shape(1, 4) == Approx( 0.08        ) );
        REQUIRE( shape(1, 5) == Approx( 0.08        ) );
    }
}

TEST_CASE( "Shape functions of a weighted Nurbs surface",
    "[SurfaceShapeEvaluator]" )
{
    using namespace ANurbs;

    const int degreeU = 2;
    const int degreeV = 1;
    const int order = 2;

    SurfaceShapeEvaluator<double> shape(degreeU, degreeV, order);
 
    std::vector<double> knotsU = {0, 0, 7.5, 15, 15.0};
    std::vector<double> knotsV = {0, 10, 20};

    Grid<double> weights(4, 3);
    weights.SetValue(0, 0, 1.0);
    weights.SetValue(0, 1, 1.0);
    weights.SetValue(0, 2, 1.0);
    weights.SetValue(1, 0, 1.0);
    weights.SetValue(1, 1, 2.5); // <<<
    weights.SetValue(1, 2, 1.0);
    weights.SetValue(2, 0, 1.0);
    weights.SetValue(2, 1, 1.0);
    weights.SetValue(2, 2, 1.0);
    weights.SetValue(3, 0, 1.0);
    weights.SetValue(3, 1, 1.0);
    weights.SetValue(3, 2, 1.0);

    const double u = 12;
    const double v = 5;

    shape.Compute(knotsU, knotsV, weights, u, v);

    CHECK( shape.DegreeU() == 2 );
    CHECK( shape.DegreeV() == 1 );
    CHECK( shape.NbShapes() == 6 );
    CHECK( shape.NbNonzeroPoles() == 6 );
    CHECK( shape.NbNonzeroPolesU() == 3 );
    CHECK( shape.NbNonzeroPolesV() == 2 );
    CHECK( shape.FirstNonzeroPoleU() == 1 );
    CHECK( shape.FirstNonzeroPoleV() == 0 );
    CHECK( shape.LastNonzeroPoleU() == 3 );
    CHECK( shape.LastNonzeroPoleV() == 1 );

    SECTION( "Check nonzero pole indices" ) {
        const auto nonZeroPoleIndices = shape.NonzeroPoleIndices();

        CHECK( nonZeroPoleIndices.size() == 6 );

        CHECK( nonZeroPoleIndices[0] == std::pair<int, int>(1, 0) );
        CHECK( nonZeroPoleIndices[1] == std::pair<int, int>(1, 1) );
        CHECK( nonZeroPoleIndices[2] == std::pair<int, int>(2, 0) );
        CHECK( nonZeroPoleIndices[3] == std::pair<int, int>(2, 1) );
        CHECK( nonZeroPoleIndices[4] == std::pair<int, int>(3, 0) );
        CHECK( nonZeroPoleIndices[5] == std::pair<int, int>(3, 1) );
    }

    SECTION( "Double index access" ) {
        REQUIRE( shape(0, 0, 0) == Approx( 0.0377358491) );
        REQUIRE( shape(0, 0, 1) == Approx( 0.0943396226) );
        REQUIRE( shape(0, 1, 0) == Approx( 0.2641509434) );
        REQUIRE( shape(0, 1, 1) == Approx( 0.2641509434) );
        REQUIRE( shape(0, 2, 0) == Approx( 0.1698113208) );
        REQUIRE( shape(0, 2, 1) == Approx( 0.1698113208) );

        REQUIRE( shape(1, 0, 0) == Approx(-0.0237332384) );
        REQUIRE( shape(1, 0, 1) == Approx(-0.0593330960) );
        REQUIRE( shape(1, 1, 0) == Approx(-0.0403465053) );
        REQUIRE( shape(1, 1, 1) == Approx(-0.0403465053) );
        REQUIRE( shape(1, 2, 0) == Approx( 0.0818796725) );
        REQUIRE( shape(1, 2, 1) == Approx( 0.0818796725) );

        // check second time

        shape.Compute(knotsU, knotsV, weights, u, v);

        REQUIRE( shape(0, 0, 0) == Approx( 0.0377358491) );
        REQUIRE( shape(0, 0, 1) == Approx( 0.0943396226) );
        REQUIRE( shape(0, 1, 0) == Approx( 0.2641509434) );
        REQUIRE( shape(0, 1, 1) == Approx( 0.2641509434) );
        REQUIRE( shape(0, 2, 0) == Approx( 0.1698113208) );
        REQUIRE( shape(0, 2, 1) == Approx( 0.1698113208) );

        REQUIRE( shape(1, 0, 0) == Approx(-0.0237332384) );
        REQUIRE( shape(1, 0, 1) == Approx(-0.0593330960) );
        REQUIRE( shape(1, 1, 0) == Approx(-0.0403465053) );
        REQUIRE( shape(1, 1, 1) == Approx(-0.0403465053) );
        REQUIRE( shape(1, 2, 0) == Approx( 0.0818796725) );
        REQUIRE( shape(1, 2, 1) == Approx( 0.0818796725) );
    }

    SECTION( "Single index access" ) {
        REQUIRE( shape(0, 0) == Approx( 0.0377358491) );
        REQUIRE( shape(0, 1) == Approx( 0.0943396226) );
        REQUIRE( shape(0, 2) == Approx( 0.2641509434) );
        REQUIRE( shape(0, 3) == Approx( 0.2641509434) );
        REQUIRE( shape(0, 4) == Approx( 0.1698113208) );
        REQUIRE( shape(0, 5) == Approx( 0.1698113208) );

        REQUIRE( shape(1, 0) == Approx(-0.0237332384) );
        REQUIRE( shape(1, 1) == Approx(-0.0593330960) );
        REQUIRE( shape(1, 2) == Approx(-0.0403465053) );
        REQUIRE( shape(1, 3) == Approx(-0.0403465053) );
        REQUIRE( shape(1, 4) == Approx( 0.0818796725) );
        REQUIRE( shape(1, 5) == Approx( 0.0818796725) );

        // check second time

        shape.Compute(knotsU, knotsV, weights, u, v);

        REQUIRE( shape(0, 0) == Approx( 0.0377358491) );
        REQUIRE( shape(0, 1) == Approx( 0.0943396226) );
        REQUIRE( shape(0, 2) == Approx( 0.2641509434) );
        REQUIRE( shape(0, 3) == Approx( 0.2641509434) );
        REQUIRE( shape(0, 4) == Approx( 0.1698113208) );
        REQUIRE( shape(0, 5) == Approx( 0.1698113208) );

        REQUIRE( shape(1, 0) == Approx(-0.0237332384) );
        REQUIRE( shape(1, 1) == Approx(-0.0593330960) );
        REQUIRE( shape(1, 2) == Approx(-0.0403465053) );
        REQUIRE( shape(1, 3) == Approx(-0.0403465053) );
        REQUIRE( shape(1, 4) == Approx( 0.0818796725) );
        REQUIRE( shape(1, 5) == Approx( 0.0818796725) );
    }
}