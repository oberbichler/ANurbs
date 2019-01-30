#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ANurbs/Data>
#include <ANurbs/Core>

#include <functional>
#include <string>
#include <unordered_map>

struct PythonDataTypeBase
{
    virtual ~PythonDataTypeBase() { }

    virtual pybind11::object
    Get(
        const ANurbs::Model& model,
        const std::string& key) = 0;

    virtual pybind11::object
    Get(
        const ANurbs::Model& model,
        const size_t key) = 0;

    virtual pybind11::object
    GetByType(
        const ANurbs::Model& model) = 0;

    static std::unordered_map<std::string, ANurbs::Unique<PythonDataTypeBase>>
        s_types;

    static pybind11::object
    ByKey(
        const ANurbs::Model& model,
        const std::string& key)
    {
        const auto type = model.GetType(key);
        return s_types[type]->Get(model, key);
    }

    static pybind11::object
    ByIndex(
        const ANurbs::Model& model,
        const size_t index)
    {
        const auto type = model.GetType(index);
        return s_types[type]->Get(model, index);
    }

    static pybind11::object
    ByType(
        const ANurbs::Model& model,
        const std::string& type)
    {
        return s_types[type]->GetByType(model);
    }
};

std::unordered_map<std::string, ANurbs::Unique<PythonDataTypeBase>>
    PythonDataTypeBase::s_types;

template <typename TData>
struct PythonDataType : public PythonDataTypeBase
{
    pybind11::object
    Get(
        const ANurbs::Model& model,
        const std::string& key) override
    {
        return pybind11::cast(model.Get<TData>(key));
    }

    pybind11::object
    Get(
        const ANurbs::Model& model,
        const size_t index) override
    {
        return pybind11::cast(model.Get<TData>(index));
    }

    pybind11::object
    GetByType(
        const ANurbs::Model& model) override
    {
        return pybind11::cast(model.GetByType<TData>());
    }

    static ANurbs::Ref<TData>
    Add(
        ANurbs::Model& self,
        ANurbs::Pointer<TData> data)
    {
        return self.Add<TData>(data);
    }

    static ANurbs::Ref<TData>
    AddWithKey(
        ANurbs::Model& self,
        const std::string& key,
        ANurbs::Pointer<TData> data)
    {
        return self.Add<TData>(key, data);
    }

    static ANurbs::Ref<TData>
    Replace(
        ANurbs::Model& self,
        const size_t index,
        ANurbs::Pointer<TData> data)
    {
        return self.Replace<TData>(index, data);
    }

    static ANurbs::Ref<TData>
    ReplaceWithKey(
        ANurbs::Model& self,
        const std::string& key,
        ANurbs::Pointer<TData> data)
    {
        return self.Replace<TData>(key, data);
    }
};

template <typename TModule>
pybind11::class_<ANurbs::Model, ANurbs::Pointer<ANurbs::Model>>
RegisterModel(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;

    using Type = Model;
    
    return pybind11::class_<Type, ANurbs::Pointer<Type>>(m, "Model")
        .def(pybind11::init<>())
        .def("Load", &Type::Load, "Path"_a)
        .def("Save", &Type::Save, "Path"_a)
        .def("AddArray", &Type::AddArray, "Content"_a)
        .def("AddObject", &Type::AddObject, "Content"_a)
        .def("NbEntries", &Type::NbEntries)
        .def("GetType", (std::string (Type::*)(const size_t) const)
            &Type::GetType, "Index"_a)
        .def("GetType", (std::string (Type::*)(const std::string&) const)
            &Type::GetType, "Key"_a)
        .def("GetKey", &Type::GetKey, "Index"_a)
        .def("Get", &PythonDataTypeBase::ByKey, "Key"_a)
        .def("Get", &PythonDataTypeBase::ByIndex, "Index"_a)
        .def("GetByType", &PythonDataTypeBase::ByType, "Type"_a)
        .def("Remove", (void (Type::*)(const size_t))
            &Type::Remove, "Index"_a)
        .def("Remove", (void (Type::*)(const std::string&))
            &Type::Remove, "Key"_a)
    ;
}

template <typename TData, typename TPyModel, typename TModule>
pybind11::class_<TData, ANurbs::Pointer<TData>>
RegisterDataTypeAndType(
    TModule& m,
    TPyModel& model,
    const std::string& name)
{
    using namespace ANurbs;
    using namespace pybind11::literals;

    Model::Register<TData>();

    pybind11::class_<Ref<TData>>(m, (std::string("Ref") + name).c_str())
        .def("Key", &Ref<TData>::Key)
        .def("Type", &Ref<TData>::Type)
        .def("Data", &Ref<TData>::Data)
        .def("Attributes", &Ref<TData>::Attributes)
    ;

    PythonDataTypeBase::s_types[TypeStringOf<TData>()] =
        New<PythonDataType<TData>>();

    model.def("Add", &PythonDataType<TData>::Add, "data"_a);

    model.def("Add", [](ANurbs::Model& self, const Ref<TData>& ref) {
        return PythonDataType<TData>::AddWithKey(self, ref.Key(), ref.Data());
    }, "ref"_a);

    model.def("Add", &PythonDataType<TData>::AddWithKey, "key"_a, "data"_a);

    model.def("Replace", &PythonDataType<TData>::Replace, "index"_a, "data"_a);

    model.def("Replace", &PythonDataType<TData>::ReplaceWithKey, "key"_a,
        "data"_a);

    return pybind11::class_<TData, Pointer<TData>>(m, name.c_str());
};

template <typename TData, typename TBase, typename TPyModel, typename TModule>
pybind11::class_<TData, TBase, ANurbs::Pointer<TData>>
RegisterDataTypeAndType(
    TModule& m,
    TPyModel& model,
    const std::string& name)
{
    using namespace ANurbs;
    using namespace pybind11::literals;

    Model::Register<TData>();

    pybind11::class_<Ref<TData>>(m, (std::string("Ref") + name).c_str())
        .def("Key", &Ref<TData>::Key)
        .def("Type", &Ref<TData>::Type)
        .def("Data", &Ref<TData>::Data)
        .def("Attributes", &Ref<TData>::Attributes)
    ;

    PythonDataTypeBase::s_types[TypeStringOf<TData>()] =
        New<PythonDataType<TData>>();

    model.def("Add", &PythonDataType<TData>::Add, "data"_a);

    model.def("Add", &PythonDataType<TData>::AddWithKey, "key"_a, "data"_a);

    model.def("Replace", &PythonDataType<TData>::Replace, "index"_a, "data"_a);

    model.def("Replace", &PythonDataType<TData>::ReplaceWithKey, "key"_a,
        "data"_a);

    return pybind11::class_<TData, TBase, Pointer<TData>>(m, name.c_str());
};

template <typename TData, typename TPyModel, typename TModule>
void
RegisterDataType(
    TModule& m,
    TPyModel& model,
    const std::string& name)
{
    using namespace ANurbs;
    using namespace pybind11::literals;

    Model::Register<TData>();

    pybind11::class_<Ref<TData>>(m, (std::string("Ref") + name).c_str())
        .def("Key", &Ref<TData>::Key)
        .def("Type", &Ref<TData>::Type)
        .def("Data", &Ref<TData>::Data)
        .def("Attributes", &Ref<TData>::Attributes)
    ;

    PythonDataTypeBase::s_types[TypeStringOf<TData>()] =
        New<PythonDataType<TData>>();

    model.def("Add", &PythonDataType<TData>::Add, "data"_a);

    model.def("Add", &PythonDataType<TData>::AddWithKey, "key"_a, "data"_a);

    model.def("Add", [](ANurbs::Model& self, const Ref<TData>& ref) {
        return PythonDataType<TData>::AddWithKey(self, ref.Key(), ref.Data());
    }, "ref"_a);
    
    model.def("Replace", &PythonDataType<TData>::Replace, "index"_a, "data"_a);

    model.def("Replace", &PythonDataType<TData>::ReplaceWithKey, "key"_a,
        "data"_a);
};

template <typename TTypeFactory, int TDimension, typename TModule,
    typename TModel>
void
RegisterCurve(
    TModule& m,
    TModel& model)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;

    using Vector = typename TTypeFactory::template Vector<double, TDimension>;
    using Geometry = ANurbs::CurveGeometry<Vector>;

    using Type = ANurbs::Curve<Geometry, Ref<CurveGeometry<Vector>>>;
    using Holder = ANurbs::Pointer<Type>;
    using Base = ANurbs::CurveBase<Vector>;

    const std::string name = "Curve" + std::to_string(TDimension) + "D";

    py::class_<Type, Base, Holder>(m, name.c_str())
        .def(py::init<Ref<Geometry>>(), "geometry"_a)
        .def("Geometry", &Type::CurveGeometry)
    ;

    RegisterDataType<Type>(m, model, name);
}

template <typename TTypeFactory, int TDimension, typename TModule>
void
RegisterCurveBase(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;

    using Vector = typename TTypeFactory::template Vector<double, TDimension>;

    using Type = ANurbs::CurveBase<Vector>;
    using Holder = ANurbs::Pointer<Type>;

    const std::string name = "CurveBase" + std::to_string(TDimension) + "D";

    py::class_<Type, Holder>(m, name.c_str())
        .def("Degree", &Type::Degree)
        .def("Domain", &Type::Domain)
        .def("PointAt", &Type::PointAt, "t"_a)
        .def("DerivativesAt", &Type::DerivativesAt, "t"_a, "order"_a)
        .def("Spans", &Type::Spans)
    ;
}

template <typename TFactory, int TDimension, typename TModule, typename TModel>
void
RegisterCurveGeometry(
    TModule& m,
    TModel& model)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;

    using Vector = typename TFactory::template Vector<double, TDimension>;

    using Type = ANurbs::CurveGeometry<Vector>;
    using Holder = ANurbs::Pointer<Type>;
    using Base = ANurbs::CurveGeometryBase<Vector>;

    const std::string name = "CurveGeometry" + std::to_string(TDimension) + "D";

    py::class_<Type, Base, Holder>(m, name.c_str())
        .def(pybind11::init<const int, const int, const bool>(), "degree"_a,
            "nbPoles"_a, "isRational"_a)
        .def("Knots", &Type::Knots)
        .def("Poles", &Type::Poles)
        .def("Weights", &Type::Weights)
        .def("Clone", &Type::Clone)
    ;

    RegisterDataType<Type>(m, model, name);
}

template <typename TTypeFactory, int TDimension, typename TModule>
void
RegisterCurveGeometryBase(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;

    using Vector = typename TTypeFactory::template Vector<double, TDimension>;

    using Type = ANurbs::CurveGeometryBase<Vector>;
    using Holder = ANurbs::Pointer<Type>;

    const std::string name = "CurveGeometryBase" + std::to_string(TDimension) +
        "D";

    py::class_<Type, Holder>(m, name.c_str())
        .def("Degree", &Type::Degree)
        .def("IsRational", &Type::IsRational)
        .def("Domain", &Type::Domain)
        .def("NbKnots", &Type::NbKnots)
        .def("Knot", &Type::Knot, "index"_a)
        .def("SetKnot", &Type::SetKnot, "index"_a, "value"_a)
        .def("Spans", &Type::Spans)
        .def("SetPole", &Type::SetPole, "index"_a, "value"_a)
        .def("Weight", &Type::Weight, "index"_a)
        .def("SetWeight", &Type::SetWeight, "index"_a, "value"_a)
        .def("PointAt", &Type::PointAt, "t"_a)
        .def("DerivativesAt", &Type::DerivativesAt, "t"_a, "order"_a)
        .def("ShapeFunctionsAt", &Type::ShapeFunctionsAt, "t"_a, "order"_a)
    ;
}

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
        .def(pybind11::init<Pointer<CurveBaseType>, double>(), "curve"_a,
            "tolerance"_a)
        .def("Compute", &Type::Compute, "point"_a)
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
        .def(pybind11::init<Pointer<SurfaceBaseType>>(), "surface"_a)
        .def("Compute", &Type::Compute, "point"_a)
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
        .def("Compute", &Type::Compute, "curve"_a, "tolerance"_a)
        .def("NbPoints", &Type::NbPoints)
        .def("Parameter", &Type::Parameter, "index"_a)
        .def("Point", &Type::Point, "index"_a)
    ;
}

template <typename TTypeFactory, int TDimension, typename TModule>
void RegisterCurveOnSurface(
    TModule& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using Vector2Type = typename TTypeFactory::template Vector<double, 2>;
    using VectorType = typename TTypeFactory::template Vector<double, TDimension>;

    using CurveGeometryBaseType = ANurbs::CurveGeometryBase<Vector2Type>;
    using SurfaceGeometryBaseType = ANurbs::SurfaceGeometryBase<VectorType>;

    using Type = ANurbs::CurveOnSurface<Vector2Type, VectorType>;
    using Pointer = ANurbs::Pointer<Type>;
    using Base = ANurbs::CurveBase<VectorType>;

    std::string name = "CurveOnSurface" + std::to_string(TDimension) + "D";

    pybind11::class_<Type, Base, Pointer>(m, name.c_str())
        .def(pybind11::init<ANurbs::Pointer<CurveGeometryBaseType>,
            ANurbs::Pointer<SurfaceGeometryBaseType>,
            ANurbs::Interval<double>>(), "curveGeometry"_a, "surfaceGeometry"_a,
            "domain"_a)
    ;
}

// template <typename TFactory, int TDimension>
// void RegisterTrimmedSurfaceClipping(
//     pybind11::module& m,
//     const std::string& name)
// {
//     namespace py = pybind11;
//     using namespace pybind11::literals;

//     using Vector2Type = Eigen::Matrix<double, 2, 1>;
//     using VectorType = typename TFactory::template Vector<double, TDimension>;

//     using Type = ANurbs::TrimmedSurfaceClipping<Vector2Type>;

//     pybind11::class_<Type>(m, name.c_str())
//         .def(py::init<double, double>(),
//             "Tolerance"_a,
//             "Scale"_a)
//         .def("Clear", &Type::Clear)
//         .def("BeginLoop", &Type::BeginLoop)
//         .def("EndLoop", &Type::EndLoop)
//         .def("AddCurve", &Type::AddCurve,
//             "Curve"_a)
//         .def("Compute", (void (Type::*)(
//             const std::vector<ANurbs::Interval<double>>&,
//             const std::vector<ANurbs::Interval<double>>&)) &Type::Compute,
//             "KnotsU"_a,
//             "KnotsV"_a)
//         .def("Compute", (void (Type::*)
//             (ANurbs::SurfaceGeometryBase<VectorType>&))
//             &Type::Compute<VectorType>,
//             "Surface"_a)
//         .def_property_readonly("NbSpansU", &Type::NbSpansU)
//         .def_property_readonly("NbSpansV", &Type::NbSpansV)
//         .def("SpanU", &Type::SpanU,
//             "Index"_a)
//         .def("SpanV", &Type::SpanV,
//             "Index"_a)
//         .def("SpanTrimType", &Type::SpanTrimType,
//             "IndexU"_a,
//             "IndexV"_a)
//         .def("SpanPolygons", &Type::SpanPolygons,
//             "IndexU"_a,
//             "IndexV"_a)
//     ;
// }

// void RegisterTriangleIndices(
//     pybind11::module& m,
//     const std::string& name)
// {
//     namespace py = pybind11;
//     using namespace pybind11::literals;

//     using Type = ANurbs::TriangleIndices;

//     pybind11::class_<Type>(m, name.c_str())
//         .def(py::init<>())
//         .def("__iter__",
//             [](const Type &self) {
//                 return pybind11::make_iterator(&self.a, &self.a + 3);
//             }, pybind11::keep_alive<0, 1>())
//         .def_readwrite("a", &Type::a)
//         .def_readwrite("b", &Type::b)
//         .def_readwrite("c", &Type::c)
//     ;
// }

// void RegisterQuadIndices(
//     pybind11::module& m,
//     const std::string& name)
// {
//     namespace py = pybind11;
//     using namespace pybind11::literals;

//     using Type = ANurbs::QuadIndices;

//     pybind11::class_<Type>(m, name.c_str())
//         .def(py::init<>())
//         .def_readwrite("a", &Type::a)
//         .def_readwrite("b", &Type::b)
//         .def_readwrite("c", &Type::c)
//         .def_readwrite("d", &Type::d)
//     ;
// }

// void RegisterPolygonTessellation(
//     pybind11::module& m,
//     const std::string& name)
// {
//     namespace py = pybind11;
//     using namespace pybind11::literals;

//     using VectorType = Eigen::Matrix<double, 2, 1>;

//     using Type = ANurbs::PolygonTessellation<VectorType>;

//     pybind11::class_<Type>(m, name.c_str())
//         .def(py::init<>())
//         .def("Compute", &Type::Compute,
//             "Polygon"_a)
//         .def_property_readonly("NbTriangles", &Type::NbTriangles)
//         .def("Triangle", &Type::Triangle,
//             "index"_a)
//     ;
// }

// void RegisterPolygonIntegrationPoints(
//     pybind11::module& m,
//     const std::string& name)
// {
//     namespace py = pybind11;
//     using namespace pybind11::literals;

//     using VectorType = Eigen::Matrix<double, 2, 1>;

//     using Type = ANurbs::PolygonIntegrationPoints<VectorType>;

//     pybind11::class_<Type>(m, name.c_str())
//         .def(py::init<>())
//         .def("Compute", &Type::Compute,
//             "Degree"_a,
//             "Polygon"_a)
//         .def_property_readonly("NbIntegrationPoints", &Type::NbIntegrationPoints)
//         .def("IntegrationPoint", &Type::IntegrationPoint,
//             "index"_a)
//     ;
// }

template <typename TTypeFactory, typename TModule>
void
RegisterData(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;

    using Vector3D = typename TTypeFactory::template Vector<double, 3>;

    // Register Model
    auto model = RegisterModel(m);


    // --- Algorithm

    { // Knots
        using Type = ANurbs::Knots;

        py::class_<Type>(m, "Knots")
            .def_static("UpperSpan", &Type::UpperSpan<double,
                std::vector<double>>)
            .def_static("LowerSpan", &Type::UpperSpan<double,
                std::vector<double>>)
            .def_static("Degree", &Type::Degree)
            .def_static("NbKnots", &Type::NbKnots)
            .def_static("NbPoles", &Type::NbPoles)
            .def_static("NbSpans", &Type::NbSpans)
        ;
    }
    
    { // CurveShapeEvaluator
        using Type = ANurbs::CurveShapeEvaluator<double>;

        pybind11::class_<Type>(m, "CurveShapeEvaluator")
            .def(pybind11::init<int, int>(), "degree"_a, "order"_a)
            .def("__call__", (double (Type::*)(const int, const int, const int)
                const) &Type::operator(), "derivative"_a, "poleU"_a, "poleV"_a)
            .def("__call__", (double (Type::*)(const int, const int) const)
                &Type::operator(), "derivative"_a, "pole"_a)
            .def("Resize", &Type::Resize, "degree"_a, "order"_a)
            .def("Degree", &Type::Degree)
            .def("Order", &Type::Order)
            .def("NbNonzeroPoles", &Type::NbNonzeroPoles)
            .def("FirstNonzeroPole", &Type::FirstNonzeroPole)
            .def("LastNonzeroPole", &Type::LastNonzeroPole)
            .def("NonzeroPoleIndices", &Type::NonzeroPoleIndices)
            .def("NbShapes", &Type::NbShapes)
            .def("Compute", &Type::Compute<std::vector<double>>, "knots"_a,
                "t"_a)
            .def("Compute", &Type::Compute<std::vector<double>,
                std::vector<double>>, "knots"_a, "weights"_a, "t"_a)
        ;
    }

    { // SurfaceShapeEvaluator
        using Type = ANurbs::SurfaceShapeEvaluator<double>;

        pybind11::class_<Type>(m, "SurfaceShapeEvaluator")
            .def(pybind11::init<int, int, int>(), "degreeU"_a, "degreeV"_a,
                "order"_a)
            .def("Resize", &Type::Resize, "degreeU"_a, "degreeV"_a, "order"_a)
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
                const) &Type::operator(), "derivative"_a, "poleU"_a, "poleV"_a)
            .def("__call__", (double (Type::*)(const int, const int) const)
                &Type::operator(), "derivative"_a, "pole"_a)
            .def("Compute", &Type::Compute<std::vector<double>>, "knotsU"_a,
                "knotsV"_a, "u"_a, "v"_a)
            .def("Compute", &Type::Compute<std::vector<double>,
                ANurbs::Grid<double>>, "knotsU"_a, "knotsV"_a, "weights"_a,
                "u"_a, "v"_a)
        ;
    }

    { // PointOnCurveProjection
        RegisterPointOnCurveProjection<TTypeFactory, 2>(m);
        RegisterPointOnCurveProjection<TTypeFactory, 3>(m);
    }

    { // PointOnSurfaceProjection
        RegisterPointOnSurfaceProjection<TTypeFactory, 3>(m);
    }

    { // CurveTessellation
        RegisterCurveTessellation<TTypeFactory, 2>(m);
        RegisterCurveTessellation<TTypeFactory, 3>(m);
    }

    { // IntegrationPoint1D
        using Type = IntegrationPoint1<double>;

        pybind11::class_<Type>(m, "IntegrationPoint1D")
            .def("__iter__", [](const Type &self) {
                    return pybind11::make_iterator(&self.t, &self.t + 2);
                }, pybind11::keep_alive<0, 1>())
            .def_readwrite("t", &Type::t)
            .def_readwrite("weight", &Type::weight)
        ;
    }
   
    { // IntegrationPoint2D
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

    { // IntegrationPoints
        using Type = IntegrationPoints<double>;

        pybind11::class_<Type>(m, "IntegrationPoints")
            .def_static("Points1D", &Type::Points1, "degree"_a, "domain"_a)
            .def_static("Points2D", (std::vector<IntegrationPoint2<double>>
                (*)(const size_t, const size_t, const Interval<double>&,
                const Interval<double>&)) &Type::Points2, "degreeU"_a,
                "degreeV"_a, "domainU"_a, "domainV"_a)
            .def_static("Points2D", (std::vector<IntegrationPoint2<double>>
                (*)(const size_t, const size_t,
                const std::vector<Interval<double>>&,
                const std::vector<Interval<double>>&)) &Type::Points2,
                "degreeU"_a, "degreeV"_a, "domainsU"_a, "domainsV"_a)
        ;
    }


    // --- Geometry

    { // Interval
        using Type = ANurbs::Interval<double>;

        py::class_<Type>(m, "Interval")
            .def(py::init<double, double>())
            .def("T0", &Type::T0)
            .def("T1", &Type::T1)
            .def("Min", &Type::Min)
            .def("Max", &Type::Max)
            .def("Delta", &Type::Delta)
            .def("Length", &Type::Length)
            .def("NormalizedAt", &Type::NormalizedAt,
                "t"_a)
            .def("ParameterAtNormalized", (double (Type::*)(const double) const)
                &Type::ParameterAtNormalized, "t"_a)
            .def("Clamp", (double (Type::*)(const double) const) &Type::Clamp,
                "t"_a)
        ;
    }

    { // CurveGeometryBase
        RegisterCurveGeometryBase<TTypeFactory, 2>(m);
        RegisterCurveGeometryBase<TTypeFactory, 3>(m);
    }
    
    { // CurveGeometry
        RegisterCurveGeometry<TTypeFactory, 2>(m, model);
        RegisterCurveGeometry<TTypeFactory, 3>(m, model);
    }

    { // CurveBase
        RegisterCurveBase<TTypeFactory, 2>(m);
        RegisterCurveBase<TTypeFactory, 3>(m);
    }

    { // Curve
        RegisterCurve<TTypeFactory, 2>(m, model);
        RegisterCurve<TTypeFactory, 3>(m, model);
    }

    { // CurveOnSurface
        RegisterCurveOnSurface<TTypeFactory, 3>(m);
    }
        
    { // Point2D
        using Vector = typename TTypeFactory::template Vector<double, 2>;
        using Type = PointData<Vector>;

        RegisterDataTypeAndType<Type>(m, model, "Point2D")
            .def(pybind11::init<Vector>(), "location"_a)
            .def("Location", &Type::Location)
            .def("SetLocation", &Type::SetLocation, "value"_a)
        ;
    }

    { // Point3D
        using Vector = typename TTypeFactory::template Vector<double, 3>;
        using Type = PointData<Vector>;

        RegisterDataTypeAndType<Type>(m, model, "Point3D")
            .def(pybind11::init<Vector>(), "location"_a)
            .def("Location", &Type::Location)
            .def("SetLocation", &Type::SetLocation, "value"_a)
        ;
    }
    

    // --- Brep

    { // Brep
        using Type = Brep;

        RegisterDataTypeAndType<Type>(m, model, "Brep")
            .def("NbFaces", &Type::NbFaces)
            .def("Face", &Type::Face, "index"_a)
            .def("NbLoops", &Type::NbLoops)
            .def("Loop", &Type::Loop, "index"_a)
            .def("NbTrims", &Type::NbTrims)
            .def("Trim", &Type::Trim, "index"_a)
            .def("NbEdges", &Type::NbEdges)
            .def("Edge", &Type::Edge, "index"_a)
        ;
    }

    { // BrepFace
        using Type = BrepFace;

        RegisterDataTypeAndType<Type>(m, model, "BrepFace")
            .def("Brep", &Type::Brep)
            .def("NbLoops", &Type::NbLoops)
            .def("Loop", &Type::Loop, "index"_a)
        ;
    }

    { // BrepLoop
        using Type = BrepLoop;

        RegisterDataTypeAndType<Type>(m, model, "BrepLoop")
            .def("Face", &Type::Face)
            .def("NbTrims", &Type::NbTrims)
            .def("Trim", &Type::Trim, "index"_a)
        ;
    }

    { // BrepTrim
        using Type = BrepTrim;

        RegisterDataTypeAndType<Type>(m, model, "BrepTrim")
            .def("Loop", &Type::Loop)
            .def("Edge", &Type::Edge)
            .def("Geometry", &Type::Geometry)
        ;
    }

    { // BrepEdge
        using Type = BrepEdge;

        RegisterDataTypeAndType<Type>(m, model, "BrepEdge")
            .def("NbTrims", &Type::NbTrims)
            .def("Trim", &Type::Trim, "index"_a)
        ;
    }


    // --- Attributes

    { // Attributes
        using Type = Attributes;

        pybind11::class_<Type, Pointer<Type>>(m, "Attributes")
            .def("HasTag", &Type::HasTag, "name"_a)
            .def("AddTag", &Type::AddTag, "name"_a)
            .def("RemoveTag", &Type::RemoveTag, "name"_a)
            .def("HasUserString", &Type::HasUserString, "name"_a)
            .def("UserString", &Type::UserString, "name"_a)
            .def("SetUserString", &Type::SetUserString, "name"_a, "value"_a)
            .def("RemoveUserString", &Type::RemoveUserString, "name"_a)
        ;
    }

    { // CadAttributes
        using Type = CadAttributes;
        using Holder = Pointer<Type>;
        using Base = Attributes;

        pybind11::class_<Type, Holder, Base>(m, "CadAttributes")
            .def("Layer", &Type::Layer)
            .def("SetLayer", &Type::SetLayer, "value"_a)
            .def("Color", &Type::Color)
            .def("SetColor", &Type::SetColor, "value"_a)
        ;
    }



    // Register SurfaceGeometry3D
    {
        using Type = SurfaceGeometry<Vector3D>;

        RegisterDataTypeAndType<Type>(m, model, "SurfaceGeometry3D")
            .def(pybind11::init<const int, const int, const int, const int,
                const bool>(), "degreeU"_a, "degreeV"_a, "nbPolesU"_a,
                "nbPolesV"_a, "isRational"_a=false)
            .def("PointAt", &Type::PointAt, "u"_a, "v"_a)
            .def("DerivativesAt", &Type::DerivativesAt, "u"_a, "v"_a, "order"_a)
            .def("DegreeU", &Type::DegreeU)
            .def("DegreeV", &Type::DegreeV)
            .def("NbKnotsU", &Type::NbKnotsU)
            .def("NbKnotsV", &Type::NbKnotsV)
            .def("KnotsU", &Type::KnotsU)
            .def("KnotsV", &Type::KnotsV)
            .def("KnotU", &Type::KnotU, "index"_a)
            .def("KnotV", &Type::KnotV, "index"_a)
            .def("SetKnotU", (void (Type::*)(const int, const double))
                &Type::SetKnotU, "index"_a, "value"_a)
            .def("SetKnotV", (void (Type::*)(const int, const double))
                &Type::SetKnotV, "index"_a, "value"_a)
            .def("NbPoles", &Type::NbPoles)
            .def("NbPolesU", &Type::NbPolesU)
            .def("NbPolesV", &Type::NbPolesV)
            .def("Pole", (Vector3D (Type::*)(const int) const) &Type::Pole,
                "index"_a)
            .def("SetPole", (void (Type::*)(const int, const Vector3D&))
                &Type::SetPole, "index"_a, "value"_a)
            .def("Pole", (Vector3D (Type::*)(const int, const int) const)
                &Type::Pole, "indexU"_a, "indexV"_a)
            .def("SetPole", (void (Type::*)(const int, const int,
                const Vector3D&)) &Type::SetPole, "indexU"_a, "indexV"_a,
                "value"_a)
            .def("Weight", (double (Type::*)(const int) const) &Type::Weight,
                "index"_a)
            .def("SetWeight", (void (Type::*)(const int, const double))
                &Type::SetWeight, "index"_a, "value"_a)
            .def("Weight", (double (Type::*)(const int, const int) const)
                &Type::Weight, "indexU"_a, "indexV"_a)
            .def("SetWeight", (void (Type::*)(const int, const int,
                const double)) &Type::SetWeight, "indexU"_a, "indexV"_a,
                "value"_a)
            .def("IsRational", &Type::IsRational)
            .def("Clone", &Type::Clone)
            .def("ShapeFunctionsAt", &Type::ShapeFunctionsAt, "u"_a, "v"_a,
                "order"_a)
        ;
    }

    // Register SurfaceBase3D
    {
        using Type = SurfaceBase<Vector3D>;
        using Holder = Pointer<Type>;

        pybind11::class_<Type, Holder>(m, "SurfaceBase3D")
            .def("DomainU", &Type::DomainU)
            .def("DomainV", &Type::DomainV)
            .def("PointAt", &Type::PointAt, "u"_a, "v"_a)
            .def("DerivativesAt", &Type::DerivativesAt, "u"_a, "v"_a, "order"_a)
            .def("SpansU", &Type::SpansU)
            .def("SpansV", &Type::SpansV)
        ;
    }

    // Register Surface3D
    {
        using Type = Surface<SurfaceGeometry<Vector3D>,
            Ref<SurfaceGeometry<Vector3D>>>;
        using Base = SurfaceBase<Vector3D>;

        RegisterDataTypeAndType<Type, Base>(m, model, "Surface3D")
            .def(pybind11::init<Ref<SurfaceGeometry<Vector3D>>>(), "geometry"_a)
            .def("Geometry", &Type::SurfaceGeometry)
        ;
    }


    // Register Line2D
    {
        using Type = Line<Point2D>;

        RegisterDataTypeAndType<Type>(m, model, "Line2D")
            .def("Start", &Type::Start)
            .def("SetStart", &Type::SetStart, "Value"_a)
            .def("End", &Type::End)
            .def("SetEnd", &Type::SetEnd, "Value"_a)
        ;
    }

    // Register Line3D
    {
        using Type = Line<Point3D>;

        RegisterDataTypeAndType<Type>(m, model, "Line3D")
            .def("Start", &Type::Start)
            .def("SetStart", &Type::SetStart, "Value"_a)
            .def("End", &Type::End)
            .def("SetEnd", &Type::SetEnd, "Value"_a)
        ;
    }

    // Register Polyline2D
    {
        using Type = Polyline<Point2D>;

        RegisterDataTypeAndType<Type>(m, model, "Polyline2D")
            .def("NbPoints", &Type::NbPoints)
            .def("Point", &Type::Point, "Index"_a)
            .def("SetPoint", &Type::SetPoint, "Index"_a, "Value"_a)
        ;
    }

    // Register Polyline3D
    {
        using Type = Polyline<Point3D>;

        RegisterDataTypeAndType<Type>(m, model, "Polyline3D")
            .def("NbPoints", &Type::NbPoints)
            .def("Point", &Type::Point, "Index"_a)
            .def("SetPoint", &Type::SetPoint, "Index"_a, "Value"_a)
        ;
    }
};