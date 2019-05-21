#pragma once

#include "ANurbs.h"

#include "Model/Model.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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
    namespace py = pybind11;

    using Type = Model;
    
    return pybind11::class_<Type, ANurbs::Pointer<Type>>(m, "Model")
        .def(py::init<>())
        .def("load", &Type::load, "Path"_a)
        .def("save", &Type::save, "Path"_a)
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
    namespace py = pybind11;

    Model::Register<TData>();

    Ref<TData>::register_python(m);

    PythonDataTypeBase::s_types[TData::type_name()] =
        new_<PythonDataType<TData>>();

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
    namespace py = pybind11;

    Model::Register<TData>();

    Ref<TData>::register_python(m);

    PythonDataTypeBase::s_types[TData::type_name()] =
        new_<PythonDataType<TData>>();

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
    namespace py = pybind11;

    Model::Register<TData>();

    Ref<TData>::register_python(m);

    PythonDataTypeBase::s_types[TData::type_name()] =
        new_<PythonDataType<TData>>();

    model.def("Add", &PythonDataType<TData>::Add, "data"_a);

    model.def("Add", &PythonDataType<TData>::AddWithKey, "key"_a, "data"_a);

    model.def("Add", [](ANurbs::Model& self, const Ref<TData>& ref) {
        return PythonDataType<TData>::AddWithKey(self, ref.Key(), ref.Data());
    }, "ref"_a);
    
    model.def("Replace", &PythonDataType<TData>::Replace, "index"_a, "data"_a);

    model.def("Replace", &PythonDataType<TData>::ReplaceWithKey, "key"_a,
        "data"_a);
};

template <typename TModule>
void
RegisterData(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;


    // --- Model

    auto model = RegisterModel(m);


    // --- Algorithm

    { // CurveSpanIntersection
        CurveSpanIntersection::register_python(m);
    }

    { // CurveTessellation
        CurveTessellation<2>::register_python(m);
        CurveTessellation<3>::register_python(m);
    }

    { // KnotVector
        ANurbs::KnotVector::register_python(m);
    }

    { // KnotRefinement
        KnotRefinement<2>::register_python(m);
        KnotRefinement<3>::register_python(m);
    }

    { // IntegrationPoints
        IntegrationPoints::register_python(m);
    }

    { // PointOnCurveProjection
        PointOnCurveProjection<2>::register_python(m);
        PointOnCurveProjection<3>::register_python(m);
    }

    { // PointOnSurfaceProjection
        PointOnSurfaceProjection<3>::register_python(m);
    }

    { // PolygonIntegrationPoints
        PolygonIntegrationPoints::register_python(m);
    }

    { // Polyline
        Polyline<2>::register_python(m, model);
        Polyline<3>::register_python(m, model);
    }

    { // PolygonTessellation
        PolygonTessellation::register_python(m);
    }

    { // TrimmedSurfaceClipping
        TrimmedSurfaceClipping::register_python(m);
    }


    // --- Geometry (base types)

    { // CurveBase
        CurveBase<2>::register_python(m);
        CurveBase<3>::register_python(m);
    }
    
    { // NurbsCurveGeometry
        NurbsCurveGeometry<2>::register_python(m, model);
        NurbsCurveGeometry<3>::register_python(m, model);
    }


    // --- Geometry

    { // Brep
        Brep::register_python(m, model);
    }

    { // BrepEdge
        BrepEdge::register_python(m, model);
    }

    { // BrepFace
        BrepFace::register_python(m, model);
    }

    { // BrepLoop
        BrepLoop::register_python(m, model);
    }

    { // BrepTrim
        BrepTrim::register_python(m, model);
    }

    { // Curve
        Curve<2>::register_python(m, model);
        Curve<3>::register_python(m, model);
    }

    { // Interval
        Interval::register_python(m, model);
    }
    
    { // Line
        Line<2>::register_python(m, model);
        Line<3>::register_python(m, model);
    }
    
    { // SurfaceBase
        SurfaceBase<2>::register_python(m);
        SurfaceBase<3>::register_python(m);
    }
    
    { // NurbsSurfaceGeometry
        NurbsSurfaceGeometry<2>::register_python(m, model);
        NurbsSurfaceGeometry<3>::register_python(m, model);
    }
    
    { // Surface
        Surface<2>::register_python(m, model);
        Surface<3>::register_python(m, model);
    }

    { // CurveOnSurface
        CurveOnSurface<3>::register_python(m);
    }

    { // Polygon
        Polygon::register_python(m);
    }


    // --- Model

    { // Attributes
        Attributes::register_python(m);
    }

    { // CadAttributes
        CadAttributes::register_python(m);
    }
};