#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ANurbs/Data>
#include <ANurbs/ANurbs.h>

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
    namespace py = pybind11;

    Model::Register<TData>();

    py::class_<Ref<TData>>(m, (std::string("Ref") + name).c_str())
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
    namespace py = pybind11;

    Model::Register<TData>();

    py::class_<Ref<TData>>(m, (std::string("Ref") + name).c_str())
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
    namespace py = pybind11;

    Model::Register<TData>();

    py::class_<Ref<TData>>(m, (std::string("Ref") + name).c_str())
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

template <int TDimension, typename TModule,
    typename TModel>
void
RegisterLine(
    TModule& m,
    TModel& model)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;

    using Vector = Vector<TDimension>;
    using Type = Line<TDimension>;
    using Holder = ANurbs::Pointer<Type>;

    const std::string name = "Line" + std::to_string(TDimension) + "D";

    py::class_<Type, Holder>(m, name.c_str())
        .def(py::init<const Vector&, const Vector&>(), "a"_a, "b"_a)
        .def("A", &Type::A)
        .def("SetA", &Type::SetA, "value"_a)
        .def("B", &Type::B)
        .def("SetB", &Type::SetB, "value"_a)
    ;
    
    RegisterDataType<Type>(m, model, name);
}

template <int TDimension, typename TModule,
    typename TModel>
void
RegisterPolyline(
    TModule& m,
    TModel& model)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;

    using Vector = Vector<TDimension>;
    using Type = Polyline<TDimension>;
    using Holder = ANurbs::Pointer<Type>;

    const std::string name = "Polyline" + std::to_string(TDimension) + "D";

    py::class_<Type, Holder>(m, name.c_str())
        .def(py::init<int>(), "nbPoints"_a)
        .def("NbPoints", &Type::NbPoints)
        .def("Point", &Type::Point, "index"_a)
        .def("SetPoint", &Type::SetPoint, "index"_a, "value"_a)
    ;
}

template <int TDimension, typename TModule,
    typename TModel>
void
RegisterTextDot(
    TModule& m,
    TModel& model)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;
    
    using Vector = Vector<TDimension>;
    using Type = TextDot<TDimension>;
    using Holder = ANurbs::Pointer<Type>;

    const std::string name = "TextDot" + std::to_string(TDimension) + "D";

    py::class_<Type, Holder>(m, name.c_str())
        .def(py::init<Vector, std::string>(), "location"_a, "text"_a)
        .def("Location", &Type::Location)
        .def("SetLocation", &Type::SetLocation, "value"_a)
        .def("Text", &Type::Text)
        .def("SetText", &Type::SetText, "value"_a)
    ;

    RegisterDataType<Type>(m, model, name);
}

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

    { // PolygonTessellation
        PolygonTessellation::register_python(m);
    }

    { // TrimmedSurfaceClipping
        TrimmedSurfaceClipping::register_python(m);
    }


    // --- Geometry

    { // Interval
        Interval::register_python(m, model);
    }

    { // CurveBase
        CurveBase<2>::register_python(m);
        CurveBase<3>::register_python(m);
    }
    
    { // NurbsCurveGeometry
        NurbsCurveGeometry<2>::register_python(m, model);
        NurbsCurveGeometry<3>::register_python(m, model);
    }

    { // Curve
        Curve<2>::register_python(m, model);
        Curve<3>::register_python(m, model);
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




    // --- Geometry
    
    { // Grid3D
        using Vector = Vector<3>;

        using Type = ANurbs::Grid<double>;

        py::class_<Type>(m, "Vector")
        ;
    }

    { // Line
        RegisterLine<2>(m, model);
        RegisterLine<3>(m, model);
    }

    { // Polyline
        RegisterPolyline<2>(m, model);
        RegisterPolyline<3>(m, model);
    }

    { // TextDot
        RegisterTextDot<2>(m, model);
        RegisterTextDot<3>(m, model);
    }
    

    // --- Brep

    { // Brep
        using Type = Brep;

        RegisterDataTypeAndType<Type>(m, model, "Brep")
            .def("NbFaces", &Type::NbFaces)
            .def("Face", &Type::Face, "index"_a)
            .def("Faces", &Type::Faces)
            .def("NbLoops", &Type::NbLoops)
            .def("Loop", &Type::Loop, "index"_a)
            .def("Loops", &Type::Loops)
            .def("NbTrims", &Type::NbTrims)
            .def("Trim", &Type::Trim, "index"_a)
            .def("Trims", &Type::Trims)
            .def("NbEdges", &Type::NbEdges)
            .def("Edge", &Type::Edge, "index"_a)
            .def("Edges", &Type::Edges)
        ;
    }

    { // BrepFace
        using Type = BrepFace;

        using Vector = Vector<3>;

        RegisterDataTypeAndType<Type>(m, model, "BrepFace")
            .def("Brep", &Type::Brep)
            .def("NbLoops", &Type::NbLoops)
            .def("Loop", &Type::Loop, "index"_a)
            .def("Loops", &Type::Loops)
            .def("Trims", [](Type& self) -> std::vector<Ref<BrepTrim>> {
                std::vector<Ref<BrepTrim>> trims;
                for (size_t i = 0; i < self.NbLoops(); i++) {
                    auto loop = self.Loop(i);
                    for (size_t j = 0; j < loop->NbTrims(); j++) {
                        trims.push_back(loop->Trim(j));
                    }
                }
                return trims;
            })
            .def("Edges", [](Type& self) -> std::vector<Ref<BrepEdge>> {
                std::vector<Ref<BrepEdge>> edges;
                for (size_t i = 0; i < self.NbLoops(); i++) {
                    auto loop = self.Loop(i);
                    for (size_t j = 0; j < loop->NbTrims(); j++) {
                        edges.push_back(loop->Trim(j)->Edge());
                    }
                }
                return edges;
            })
            .def("Geometry", &Type::Geometry)
            // .def("UntrimmedSurface", [](Type& self) {
            //     return New<Surface<NurbsSurfaceGeometry<Vector>>>(
            //         self.Geometry().Data());
            // })
        ;
    }

    { // BrepLoop
        using Type = BrepLoop;

        RegisterDataTypeAndType<Type>(m, model, "BrepLoop")
            .def("Brep", [](Type& self) -> Ref<Brep> {
                return self.Face()->Brep();
            })
            .def("Face", &Type::Face)
            .def("NbTrims", &Type::NbTrims)
            .def("Trim", &Type::Trim, "index"_a)
            .def("Trims", &Type::Trims)
            .def("Edges", [](Type& self) -> std::vector<Ref<BrepEdge>> {
                std::vector<Ref<BrepEdge>> edges(self.NbTrims());
                for (size_t i = 0; i < edges.size(); i++) {
                    edges[i] = self.Trim(i)->Edge();
                }
                return edges;
            })
        ;
    }

    { // BrepTrim
        using Type = BrepTrim;

        RegisterDataTypeAndType<Type>(m, model, "BrepTrim")
            .def("Brep", [](Type& self) -> Ref<Brep> {
                return self.Loop()->Face()->Brep();
            })
            .def("Loop", &Type::Loop)
            .def("Edge", &Type::Edge)
            .def("Face", [](Type& self) -> Ref<BrepFace> {
                return self.Loop()->Face();
            })
            .def("Geometry", &Type::Geometry)
            .def("Curve2D", [](Type& self) {
                return New<Curve<2>>(
                    self.Geometry().Data(), self.Domain());
            })
            // .def("Curve3D", [](Type& self) {
            //     return New<CurveOnSurface<3>>(
            //         self.Geometry().Data(),
            //         self.Loop()->Face()->Geometry().Data(), self.Domain());
            // })
        ;
    }

    { // BrepEdge
        using Type = BrepEdge;

        RegisterDataTypeAndType<Type>(m, model, "BrepEdge")
            .def("NbTrims", &Type::NbTrims)
            .def("Trim", &Type::Trim, "index"_a)
            .def("Trims", &Type::Trims)
            .def("Loops", [](Type& self) -> std::vector<Ref<BrepLoop>> {
                std::vector<Ref<BrepLoop>> loops(self.NbTrims());
                for (size_t i = 0; i < loops.size(); i++) {
                    loops[i] = self.Trim(i)->Loop();
                }
                return loops;
            })
            .def("Faces", [](Type& self) -> std::vector<Ref<BrepFace>> {
                std::vector<Ref<BrepFace>> faces(self.NbTrims());
                for (size_t i = 0; i < faces.size(); i++) {
                    faces[i] = self.Trim(i)->Loop()->Face();
                }
                return faces;
            })
        ;
    }


    // --- Attributes

    { // Attributes
        using Type = Attributes;

        py::class_<Type, Pointer<Type>>(m, "Attributes")
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

        py::class_<Type, Holder, Base>(m, "CadAttributes")
            .def("Layer", &Type::Layer)
            .def("SetLayer", &Type::SetLayer, "value"_a)
            .def("Color", &Type::Color)
            .def("SetColor", &Type::SetColor, "value"_a)
            .def("Arrowhead", &Type::Arrowhead)
            .def("SetArrowhead", &Type::SetArrowhead, "value"_a)
        ;
    }
};