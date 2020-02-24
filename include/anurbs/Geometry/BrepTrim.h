#pragma once

#include "Brep.h"
#include "BrepFace.h"
#include "BrepLoop.h"
#include "BrepEdge.h"
#include "Curve.h"
#include "CurveOnSurface.h"
#include "NurbsCurveGeometry.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace anurbs {

class Brep;
class BrepEdge;
class BrepFace;
class BrepLoop;

class BrepTrim
{
private:
    Ref<BrepLoop> m_loop;
    Ref<BrepEdge> m_edge;
    Ref<NurbsCurveGeometry<2>> m_curve_geometry;
    Interval m_domain;

public:
    Ref<Brep> brep() const;

    Ref<BrepLoop> loop() const;

    Ref<BrepEdge> edge() const;

    Ref<BrepFace> face() const;

    Ref<NurbsCurveGeometry<2>> curve_geometry() const;

    Pointer<Curve<2>> curve_2d() const;

    Pointer<CurveOnSurface<3>> curve_3d() const;

    Interval domain() const;

    Ref<NurbsSurfaceGeometry<3>> surface_geometry() const;

public:     // serialization
    static std::string type_name();

    static Unique<BrepTrim> load(Model& model, const Json& data);

    static void save(const Model& model, const BrepTrim& data, Json& target);

public:     // python
    static std::string python_name();

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepTrim;
        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            // read-only properties
            .def_property_readonly("brep", &Type::brep)
            .def_property_readonly("curve_2d", &Type::curve_2d)
            .def_property_readonly("curve_3d", &Type::curve_3d)
            .def_property_readonly("curve_geometry", &Type::curve_geometry)
            .def_property_readonly("edge", &Type::edge)
            .def_property_readonly("face", &Type::face)
            .def_property_readonly("loop", &Type::loop)
            .def_property_readonly("surface_geometry", &Type::surface_geometry)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs