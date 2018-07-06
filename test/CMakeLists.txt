add_executable(run_tests
    main.cc
    TestPoint.cc
    TestCurveShapeEvaluator.cc
    TestCurveGeometry.cc
    TestCurve.cc
)

install(TARGETS run_tests DESTINATION bin)
