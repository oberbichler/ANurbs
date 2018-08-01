add_executable(run_tests
    main.cc
    TestPoint.cc
    TestCurveShapeEvaluator.cc
    TestCurveGeometry.cc
    TestCurve.cc
    TestCurveTessellation.cc
    TestSurfaceShapeEvaluator.cc
    TestSurfaceGeometry.cc
    TestSurface.cc
    TestIntegrationPoints.cc
)

install(TARGETS run_tests DESTINATION bin)
