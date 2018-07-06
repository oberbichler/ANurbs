#include "catch.hpp"

#include <ANurbs/src/Point.h>

using namespace ANurbs;


TEST_CASE( "Create a Point2D by initializer list", "[Point]" ) {
    Point<double, 2> point = {1.2, 3.4};

    SECTION( "Access values by index" ) {
        CHECK( point[0] == 1.2 );
        CHECK( point[1] == 3.4 );
    }

    SECTION( "Access values by XY" ) {
        CHECK( point.X() == 1.2 );
        CHECK( point.Y() == 3.4 );
    }
}

TEST_CASE( "Assign Point2D by index", "[Point]" ) {
    Point<double, 2> point;
    point[0] = 1.2;
    point[1] = 3.4;

    SECTION( "Access values by index" ) {
        CHECK( point[0] == 1.2 );
        CHECK( point[1] == 3.4 );
    }

    SECTION( "Access values by XY" ) {
        CHECK( point.X() == 1.2 );
        CHECK( point.Y() == 3.4 );
    }
}

TEST_CASE( "Assign Point2D by XY", "[Point]" ) {
    Point<double, 2> point;
    point.X() = 1.2;
    point.Y() = 3.4;

    SECTION( "Access values by index" ) {
        CHECK( point[0] == 1.2 );
        CHECK( point[1] == 3.4 );
    }

    SECTION( "Access values by XY" ) {
        CHECK( point.X() == 1.2 );
        CHECK( point.Y() == 3.4 );
    }
}

TEST_CASE( "Point3D by initializer list", "[Point]" ) {
    Point<double, 3> point = {1.2, 3.4, 5.6};

    SECTION( "Access values by index" ) {
        CHECK( point[0] == 1.2 );
        CHECK( point[1] == 3.4 );
        CHECK( point[2] == 5.6 );
    }

    SECTION( "Access values by XYZ" ) {
        CHECK( point.X() == 1.2 );
        CHECK( point.Y() == 3.4 );
        CHECK( point.Z() == 5.6 );
    }
}

TEST_CASE( "Assign Point3D by index", "[Point]" ) {
    Point<double, 3> point;
    point[0] = 1.2;
    point[1] = 3.4;
    point[2] = 5.6;

    SECTION( "Access values by index" ) {
        CHECK( point[0] == 1.2 );
        CHECK( point[1] == 3.4 );
        CHECK( point[2] == 5.6 );
    }

    SECTION( "Access values by XYZ" ) {
        CHECK( point.X() == 1.2 );
        CHECK( point.Y() == 3.4 );
        CHECK( point.Z() == 5.6 );
    }
}

TEST_CASE( "Assign Point3D by XYZ", "[Point]" ) {
    Point<double, 3> point;
    point.X() = 1.2;
    point.Y() = 3.4;
    point.Z() = 5.6;

    SECTION( "Access values by index" ) {
        CHECK( point[0] == 1.2 );
        CHECK( point[1] == 3.4 );
        CHECK( point[2] == 5.6 );
    }

    SECTION( "Access values by XYZ" ) {
        CHECK( point.X() == 1.2 );
        CHECK( point.Y() == 3.4 );
        CHECK( point.Z() == 5.6 );
    }
}

// TEST_CASE( "Accessing a invalid coordinate produces an error", "[Point]" ) {
//     Point<double, 2> point;
    
//     SECTION( "Assign negative index" ) {
//         CHECK_THROWS( point[-1] =  0.0 );
//     }
    
//     SECTION( "Get negative index" ) {
//         CHECK_THROWS( point[-1] == 0.0 );
//     }
    
//     SECTION( "Assign index out of range" ) {
//         CHECK_THROWS( point[2] =  0.0 );
//     }
    
//     SECTION( "Assign index out of range" ) {
//         CHECK_THROWS( point[2] == 0.0 );
//     }
    
//     SECTION( "Assign X" ) {
//         CHECK_THROWS( point.Z() =  0.0 );
//     }
    
//     SECTION( "Get Z" ) {
//         CHECK_THROWS( point.Z() == 0.0 );
//     }
// }
