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

    model.def("Add", &PythonDataType<TData>::Add, "Data"_a);

    model.def("Add", &PythonDataType<TData>::AddWithKey, "Key"_a, "Data"_a);

    model.def("Replace", &PythonDataType<TData>::Replace, "Index"_a, "Data"_a);

    model.def("Replace", &PythonDataType<TData>::ReplaceWithKey, "Key"_a,
        "Data"_a);

    return pybind11::class_<TData, Pointer<TData>>(m, name.c_str())
        .def(pybind11::init<TData>(), "Other"_a)
        .def("Clone", [](const TData& self) { return New<TData>(self); })
    ;
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

    model.def("Add", &PythonDataType<TData>::Add, "Data"_a);

    model.def("Add", &PythonDataType<TData>::AddWithKey, "Key"_a, "Data"_a);

    model.def("Replace", &PythonDataType<TData>::Replace, "Index"_a, "Data"_a);

    model.def("Replace", &PythonDataType<TData>::ReplaceWithKey, "Key"_a,
        "Data"_a);

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

    model.def("Add", &PythonDataType<TData>::Add, "Data"_a);

    model.def("Add", &PythonDataType<TData>::AddWithKey, "Key"_a, "Data"_a);

    model.def("Replace", &PythonDataType<TData>::Replace, "Index"_a, "Data"_a);

    model.def("Replace", &PythonDataType<TData>::ReplaceWithKey, "Key"_a,
        "Data"_a);
};

template <typename TTypeFactory, typename TModule>
void
RegisterData(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;

    using Vector2D = typename TTypeFactory::template Vector<double, 2>;
    using Vector3D = typename TTypeFactory::template Vector<double, 3>;

    // Register Model
    auto model = RegisterModel(m);

    // Register Attributes
    {
        using Type = Attributes;

        pybind11::class_<Type, Pointer<Type>>(m, "Attributes")
            .def("HasTag", &Type::HasTag, "Name"_a)
            .def("AddTag", &Type::AddTag, "Name"_a)
            .def("RemoveTag", &Type::RemoveTag, "Name"_a)
            .def("HasUserString", &Type::HasUserString, "Name"_a)
            .def("UserString", &Type::UserString, "Name"_a)
            .def("SetUserString", &Type::SetUserString, "Name"_a, "Value"_a)
            .def("RemoveUserString", &Type::RemoveUserString, "Name"_a)
        ;
    }

    // Register CadAttributes
    {
        using Type = CadAttributes;
        using Holder = Pointer<Type>;
        using Base = Attributes;

        pybind11::class_<Type, Holder, Base>(m, "CadAttributes")
            .def("Layer", &Type::Layer)
            .def("SetLayer", &Type::SetLayer, "Value"_a)
            .def("Color", &Type::Color)
            .def("SetColor", &Type::SetColor, "Value"_a)
        ;
    }

    // Register Point2D
    {
        using Type = PointData<Vector2D>;

        RegisterDataTypeAndType<Type>(m, model, "Point2D")
            .def(pybind11::init<Vector2D>(), "Location"_a)
            .def("Location", &Type::Location)
            .def("SetLocation", &Type::SetLocation, "Value"_a)
        ;
    }

    // Register Point3D
    {
        using Type = PointData<Vector3D>;

        RegisterDataTypeAndType<Type>(m, model, "Point3D")
            .def(pybind11::init<Vector3D>(), "Location"_a)
            .def("Location", &Type::Location)
            .def("SetLocation", &Type::SetLocation, "Value"_a)
        ;
    }

    // Register CurveGeometry2D
    {
        using Type = CurveGeometry<Vector2D>;

        RegisterDataTypeAndType<Type>(m, model, "CurveGeometry2D")
            .def("PointAt", &Type::PointAt, "T"_a)
            .def("SetPole", &Type::SetPole, "Index"_a, "Value"_a)
        ;
    }

    // Register CurveGeometry3D
    {
        using Type = CurveGeometry<Vector3D>;

        RegisterDataTypeAndType<Type>(m, model, "CurveGeometry3D")
            .def("PointAt", &Type::PointAt, "T"_a)
        ;
    }

    // Register CurveBase2D
    {
        using Type = ANurbs::CurveBase<Vector2D>;
        using Holder = ANurbs::Pointer<Type>;

        pybind11::class_<Type, Holder>(m, "CurveBase2D")
            .def("Domain", &Type::Domain)
            .def("PointAt", &Type::PointAt, "T"_a)
            .def("DerivativesAt", &Type::DerivativesAt, "T"_a, "Order"_a)
            .def("Spans", &Type::Spans)
        ;
    }

    // Register CurveBase3D
    {
        using Type = ANurbs::CurveBase<Vector3D>;
        using Holder = ANurbs::Pointer<Type>;

        pybind11::class_<Type, Holder>(m, "CurveBase3D")
            .def("Domain", &Type::Domain)
            .def("PointAt", &Type::PointAt, "T"_a)
            .def("DerivativesAt", &Type::DerivativesAt, "T"_a, "Order"_a)
            .def("Spans", &Type::Spans)
        ;
    }

    // Register Curve2D
    {
        using Type = Curve<CurveGeometry<Vector2D>,
            Ref<CurveGeometry<Vector2D>>>;
        using Base = CurveBase<Vector2D>;

        RegisterDataTypeAndType<Type, Base>(m, model, "Curve2D")
            .def("Geometry", &Type::CurveGeometry)
        ;
    }

    // Register Curve3D
    {
        using Type = Curve<CurveGeometry<Vector3D>,
            Ref<CurveGeometry<Vector3D>>>;
        using Base = CurveBase<Vector3D>;

        RegisterDataTypeAndType<Type, Base>(m, model, "Curve3D")
            .def("Geometry", &Type::CurveGeometry)
        ;
    }

    // Register SurfaceGeometry3D
    {
        using Type = SurfaceGeometry<Vector3D>;

        RegisterDataTypeAndType<Type>(m, model, "SurfaceGeometry3D")
            .def("PointAt", &Type::PointAt, "U"_a, "V"_a)
        ;
    }

    // Register SurfaceBase3D
    {
        using Type = SurfaceBase<Vector3D>;
        using Holder = Pointer<Type>;

        pybind11::class_<Type, Holder>(m, "SurfaceBase3D")
            .def("DomainU", &Type::DomainU)
            .def("DomainV", &Type::DomainV)
            .def("PointAt", &Type::PointAt, "U"_a, "V"_a)
            .def("DerivativesAt", &Type::DerivativesAt, "U"_a, "V"_a, "Order"_a)
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
            .def("Geometry", &Type::SurfaceGeometry)
        ;
    }

    // Register Brep
    {
        using Type = Brep;

        RegisterDataTypeAndType<Type>(m, model, "Brep")
            .def("NbFaces", &Type::NbFaces)
            .def("Face", &Type::Face, "Index"_a)
            .def("NbLoops", &Type::NbLoops)
            .def("Loop", &Type::Loop, "Index"_a)
            .def("NbTrims", &Type::NbTrims)
            .def("Trim", &Type::Trim, "Index"_a)
            .def("NbEdges", &Type::NbEdges)
            .def("Edge", &Type::Edge, "Index"_a)
        ;
    }

    // Register BrepFace
    {
        using Type = BrepFace;

        RegisterDataTypeAndType<Type>(m, model, "BrepFace")
            .def("Brep", &Type::Brep)
            .def("NbLoops", &Type::NbLoops)
            .def("Loop", &Type::Loop, "Index"_a)
        ;
    }

    // Register BrepLoop
    {
        using Type = BrepLoop;

        RegisterDataTypeAndType<Type>(m, model, "BrepLoop")
            .def("Face", &Type::Face)
            .def("NbTrims", &Type::NbTrims)
            .def("Trim", &Type::Trim, "Index"_a)
        ;
    }

    // Register BrepTrim
    {
        using Type = BrepTrim;

        RegisterDataTypeAndType<Type>(m, model, "BrepTrim")
            .def("Loop", &Type::Loop)
            .def("Edge", &Type::Edge)
            .def("Geometry", &Type::Geometry)
        ;
    }

    // Register BrepEdge
    {
        using Type = BrepEdge;

        RegisterDataTypeAndType<Type>(m, model, "BrepEdge")
            .def("NbTrims", &Type::NbTrims)
            .def("Trim", &Type::Trim, "Index"_a)
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