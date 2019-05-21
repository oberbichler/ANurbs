#pragma once

#include "BrepEdge.h"

#include "../Geometry/NurbsCurveGeometry.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

class BrepLoop;

class BrepTrim
{
private:
    Ref<BrepLoop> m_loop;
    Ref<BrepEdge> m_edge;
    Ref<NurbsCurveGeometry<2>> m_geometry;
    Interval m_domain;

public:
    Ref<BrepLoop> loop()
    {
        return m_loop;
    }

    Ref<BrepEdge> edge()
    {
        return m_edge;
    }

    Ref<NurbsCurveGeometry<2>> Geometry()
    {
        return m_geometry;
    }

    Interval Domain()
    {
        return m_domain;
    }

public:     // serialization
    using Attributes = Attributes;

    static std::string type_name()
    {
        return "BrepTrim";
    }

    static Unique<BrepTrim> load(Model& model, const Json& data)
    {
        auto result = New<BrepTrim>();

        // Read Loop
        {
            const std::string key = data.at("Loop");
            result->m_loop = model.GetLazy<BrepLoop>(key);
        }

        // Read Edge
        if (data.find("Edge") != data.end()) {
            const std::string key = data.at("Edge");
            result->m_edge = model.GetLazy<BrepEdge>(key);
        }

        // Read Geometry
        {
            const std::string key = data.at("Geometry");
            result->m_geometry = model.GetLazy<NurbsCurveGeometry<2>>(key);
        }

        // Read Domain
        if (data.find("Domain") != data.end()) {
            result->m_domain = data.at("Domain");
        } else {
            result->m_domain = result->m_geometry->domain();
        }

        return result;
    }

    static void save(const Model& model, const BrepTrim& data, Json& target)
    {
        target["Loop"] = ToJson(data.m_loop);
        if (!data.m_edge.IsEmpty()) {
            target["Edge"] = ToJson(data.m_edge);
        }
        target["Geometry"] = ToJson(data.m_geometry);
        target["Domain"] = ToJson(data.m_domain);
    }

public:     // python
    template <typename TModule, typename TModel>
    static void register_python(TModule& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepTrim;

        py::class_<Type>(m, "BrepTrim")
            // .def("Brep", [](Type& self) -> Ref<Brep> {
            //     return self.loop()->face()->Brep();
            // })
            // .def("Loop", &Type::loop)
            // .def("Edge", &Type::edge)
            // .def("Face", [](Type& self) -> Ref<BrepFace> {
            //     return self.loop()->face();
            // })
            // .def("Geometry", &Type::Geometry)
            // .def("Curve2D", [](Type& self) {
            //     return New<Curve<2>>(
            //         self.Geometry().Data(), self.Domain());
            // })
            // .def("Curve3D", [](Type& self) {
            //     return New<CurveOnSurface<3>>(
            //         self.Geometry().Data(),
            //         self.loop()->face()->Geometry().Data(), self.Domain());
            // })
        ;
    }
};

} // namespace ANurbs