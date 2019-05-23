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
    Ref<Brep> brep();

    Ref<BrepLoop> loop();

    Ref<BrepEdge> edge();

    Ref<BrepFace> face();

    Ref<NurbsCurveGeometry<2>> curve_geometry();

    Pointer<Curve<2>> curve_2d();

    Pointer<CurveOnSurface<3>> curve_3d();

    Interval domain();

    Ref<NurbsSurfaceGeometry<3>> surface_geometry();

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
            .def("brep", &Type::brep)
            .def("loop", &Type::loop)
            .def("edge", &Type::edge)
            .def("face", &Type::face)
            .def("curve_geometry", &Type::curve_geometry)
            // .def("Curve2D", [](Type& self) {
            //     return new_<Curve<2>>(
            //         self.Geometry().data(), self.Domain());
            // })
            // .def("Curve3D", [](Type& self) {
            //     return new_<CurveOnSurface<3>>(
            //         self.Geometry().data(),
            //         self.loop()->face()->Geometry().data(), self.Domain());
            // })
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace ANurbs