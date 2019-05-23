#pragma once

#include "BrepTrim.h"

#include "BrepEdge.h"
#include "BrepFace.h"
#include "BrepLoop.h"
#include "Curve.h"
#include "CurveOnSurface.h"
#include "NurbsCurveGeometry.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

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
    using Attributes = Attributes;

    static std::string type_name()
    {
        return "BrepTrim";
    }

    static Unique<BrepTrim> load(Model& model, const Json& data)
    {
        auto result = new_<BrepTrim>();

        // Read Loop
        {
            const std::string key = data.at("Loop");
            result->m_loop = model.get_lazy<BrepLoop>(key);
        }

        // Read Edge
        if (data.find("Edge") != data.end()) {
            const std::string key = data.at("Edge");
            result->m_edge = model.get_lazy<BrepEdge>(key);
        }

        // Read Geometry
        {
            const std::string key = data.at("Geometry");
            result->m_curve_geometry = model.get_lazy<NurbsCurveGeometry<2>>(key);
        }

        // Read Domain
        if (data.find("Domain") != data.end()) {
            result->m_domain = data.at("Domain");
        } else {
            result->m_domain = result->m_curve_geometry->domain();
        }

        return result;
    }

    static void save(const Model& model, const BrepTrim& data, Json& target)
    {
        target["Loop"] = ToJson(data.m_loop);
        if (!data.m_edge.is_empty()) {
            target["Edge"] = ToJson(data.m_edge);
        }
        target["Geometry"] = ToJson(data.m_curve_geometry);
        target["Domain"] = ToJson(data.m_domain);
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepTrim;

        py::class_<Type>(m, "BrepTrim")
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

} // namespace ANurbs