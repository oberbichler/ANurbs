#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ANurbs/Data>
#include <ANurbs/Core>

#include <functional>
#include <string>
#include <unordered_map>

template <typename TTypeFactory, int TDimension, typename TModule>
void
RegisterPointOnCurveProjection(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;

    using Vector = typename TTypeFactory::template Vector<double, TDimension>;

    using CurveBaseType = CurveBase<Vector>;

    using Type = PointOnCurveProjection<Vector>;
    using Handler = Pointer<Type>;

    std::string name = "PointOnCurveProjection" + std::to_string(TDimension) +
        "D";

    pybind11::class_<Type, Handler>(m, name.c_str())
        .def(pybind11::init<Pointer<CurveBaseType>, double>(), "Curve"_a,
            "Tolerance"_a)
        .def("Compute", &Type::Compute, "Point"_a)
        .def("Parameter", &Type::Parameter)
        .def("Point", &Type::Point)
    ;
}

template <typename TTypeFactory, int TDimension, typename TModule>
void
RegisterPointOnSurfaceProjection(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;

    using Vector = typename TTypeFactory::template Vector<double, TDimension>;

    using SurfaceBaseType = ANurbs::SurfaceBase<Vector>;

    using Type = PointOnSurfaceProjection<Vector>;
    using Handler = Pointer<Type>;

    std::string name = "PointOnSurfaceProjection" + std::to_string(TDimension) +
        "D";

    pybind11::class_<Type, Handler>(m, name.c_str())
        .def(pybind11::init<Pointer<SurfaceBaseType>>(), "Surface"_a)
        .def("Compute", &Type::Compute, "Point"_a)
        .def("ParameterU", &Type::ParameterU)
        .def("ParameterV", &Type::ParameterV)
        .def("Point", &Type::Point)
        .def("Distance", &Type::Distance)
    ;
}

template <typename TTypeFactory, int TDimension, typename TModule>
void
RegisterCurveTessellation(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;

    using Vector = typename TTypeFactory::template Vector<double, TDimension>;

    using Type = CurveTessellation<Vector>;
    using Handler = Pointer<Type>;

    std::string name = "CurveTessellation" + std::to_string(TDimension) + "D";

    pybind11::class_<Type, Handler>(m, name.c_str())
        .def(pybind11::init<>())
        .def("Compute", &Type::Compute, "Curve"_a, "Tolerance"_a)
        .def("NbPoints", &Type::NbPoints)
        .def("Parameter", &Type::Parameter, "Index"_a)
        .def("Point", &Type::Point, "Index"_a)
    ;
}

template <typename TTypeFactory, typename TModule>
void
RegisterAlgorithms(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;

    using Vector2D = typename TTypeFactory::template Vector<double, 2>;
    using Vector3D = typename TTypeFactory::template Vector<double, 3>;

    // Register CurveShapeEvaluator
    {
        using Type = ANurbs::CurveShapeEvaluator<double>;

        pybind11::class_<Type>(m, "CurveShapeEvaluator")
            .def(pybind11::init<int, int>(), "Degree"_a, "Order"_a)
            .def("__call__", (double (Type::*)(const int, const int, const int)
                const) &Type::operator(), "Derivative"_a, "PoleU"_a, "PoleV"_a)
            .def("__call__", (double (Type::*)(const int, const int) const)
                &Type::operator(), "Derivative"_a, "Pole"_a)
            .def("Resize", &Type::Resize, "Degree"_a, "Order"_a)
            .def("Degree", &Type::Degree)
            .def("Order", &Type::Order)
            .def("NbNonzeroPoles", &Type::NbNonzeroPoles)
            .def("FirstNonzeroPole", &Type::FirstNonzeroPole)
            .def("LastNonzeroPole", &Type::LastNonzeroPole)
            .def("NbShapes", &Type::NbShapes)
            .def("Compute", &Type::Compute<std::vector<double>>, "Knots"_a,
                "T"_a)
            .def("Compute", &Type::Compute<std::vector<double>,
                std::vector<double>>, "Knots"_a, "Weights"_a, "T"_a)
        ;
    }

    // Register SurfaceShapeEvaluator
    {
        using Type = ANurbs::SurfaceShapeEvaluator<double>;

        pybind11::class_<Type>(m, "SurfaceShapeEvaluator")
            .def(pybind11::init<int, int, int>(), "DegreeU"_a, "DegreeV"_a,
                "Order"_a)
            .def("Resize", &Type::Resize, "DegreeU"_a, "DegreeV"_a, "Order"_a)
            .def("DegreeU", &Type::DegreeU)
            .def("DegreeV", &Type::DegreeV)
            .def("Order", &Type::Order)
            .def("NbShapes", (int (Type::*)(void) const) &Type::NbShapes)
            .def("NbNonzeroPolesU", &Type::NbNonzeroPolesU)
            .def("NbNonzeroPolesV", &Type::NbNonzeroPolesV)
            .def("NbNonzeroPoles", &Type::NbNonzeroPoles)
            .def("FirstNonzeroPoleU", &Type::FirstNonzeroPoleU)
            .def("FirstNonzeroPoleV", &Type::FirstNonzeroPoleV)
            .def("LastNonzeroPoleU", &Type::LastNonzeroPoleU)
            .def("LastNonzeroPoleV", &Type::LastNonzeroPoleV)
            .def("NonzeroPoleIndices", &Type::NonzeroPoleIndices)
            .def("__call__", (double (Type::*)(const int, const int, const int)
                const) &Type::operator(), "Derivative"_a, "PoleU"_a, "PoleV"_a)
            .def("__call__", (double (Type::*)(const int, const int) const)
                &Type::operator(), "Derivative"_a, "Pole"_a)
            .def("Compute", &Type::Compute<std::vector<double>>, "KnotsU"_a,
                "KnotsV"_a, "U"_a, "V"_a)
            .def("Compute", &Type::Compute<std::vector<double>,
                ANurbs::Grid<double>>, "KnotsU"_a, "KnotsV"_a, "Weights"_a,
                "U"_a, "V"_a)
        ;
    }

    // Register PointOnCurveProjection
    RegisterPointOnCurveProjection<TTypeFactory, 2>(m);
    RegisterPointOnCurveProjection<TTypeFactory, 3>(m);

    // Register PointOnSurfaceProjection
    RegisterPointOnSurfaceProjection<TTypeFactory, 3>(m);

    // Register CurveTessellation
    RegisterCurveTessellation<TTypeFactory, 2>(m);
    RegisterCurveTessellation<TTypeFactory, 3>(m);

    // Register IntegrationPoint1D
    {
        using Type = IntegrationPoint1<double>;

        pybind11::class_<Type>(m, "IntegrationPoint1D")
            .def("__iter__", [](const Type &self) {
                    return pybind11::make_iterator(&self.t, &self.t + 2);
                }, pybind11::keep_alive<0, 1>())
            .def_readwrite("t", &Type::t)
            .def_readwrite("weight", &Type::weight)
        ;
    }

    // Register IntegrationPoint2D
    {
        using Type = ANurbs::IntegrationPoint2<double>;

        pybind11::class_<Type>(m, "IntegrationPoint2D")
            .def("__iter__", [](const Type &self) {
                    return pybind11::make_iterator(&self.u, &self.u + 3);
                }, pybind11::keep_alive<0, 1>())
            .def_readwrite("u", &Type::u)
            .def_readwrite("v", &Type::v)
            .def_readwrite("weight", &Type::weight)
        ;
    }

    // IntegrationPoints
    {
        using Type = IntegrationPoints<double>;

        pybind11::class_<Type>(m, "IntegrationPoints")
            .def_static("Points1D", &Type::Points1, "Degree"_a, "Domain"_a)
            .def_static("Points2D", (std::vector<IntegrationPoint2<double>>
                (*)(const size_t, const size_t, const Interval<double>&,
                const Interval<double>&)) &Type::Points2, "DegreeU"_a,
                "DegreeV"_a, "DomainU"_a, "DomainV"_a)
            .def_static("Points2D", (std::vector<IntegrationPoint2<double>>
                (*)(const size_t, const size_t,
                const std::vector<Interval<double>>&,
                const std::vector<Interval<double>>&)) &Type::Points2,
                "DegreeU"_a, "DegreeV"_a, "DomainsU"_a, "DomainsV"_a)
        ;
    }
}