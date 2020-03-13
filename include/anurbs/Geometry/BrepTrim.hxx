#pragma once

#include "BrepTrim.h"

#include "BrepEdge.h"
#include "Curve.h"
#include "CurveOnSurface.h"
#include "NurbsCurveGeometry.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace anurbs {

Ref<Brep> BrepTrim::brep() const
{
    return m_loop->face()->brep();
}

Pointer<Curve<2>> BrepTrim::curve_2d() const
{
    return new_<Curve<2>>(curve_geometry().data(), domain());
}

Pointer<CurveOnSurface<3>> BrepTrim::curve_3d() const
{
    return new_<CurveOnSurface<3>>(curve_geometry().data(),
        face()->surface_geometry().data(), domain());
}

Ref<NurbsCurveGeometry<2>> BrepTrim::curve_geometry() const
{
    return m_curve_geometry;
}

Interval BrepTrim::domain() const
{
    return m_domain;
}

Ref<BrepEdge> BrepTrim::edge() const
{
    return m_edge;
}

Ref<BrepFace> BrepTrim::face() const
{
    return m_loop->face();
}

Ref<BrepLoop> BrepTrim::loop() const
{
    return m_loop;
}

Ref<NurbsSurfaceGeometry<3>> BrepTrim::surface_geometry() const
{
    return face()->surface_geometry();
}

// serialization

std::string BrepTrim::type_name()
{
    return "BrepTrim";
}

Unique<BrepTrim> BrepTrim::load(Model& model, const Json& data)
{
    auto result = new_<BrepTrim>();

    // Read Loop
    {
        const std::string key = data.at("loop");
        result->m_loop = model.get_lazy<BrepLoop>(key);
    }

    // Read Edge
    if (data.find("edge") != data.end()) {
        const std::string key = data.at("edge");
        result->m_edge = model.get_lazy<BrepEdge>(key);
    }

    // Read Geometry
    {
        const std::string key = data.at("geometry");
        result->m_curve_geometry = model.get_lazy<NurbsCurveGeometry<2>>(key);
    }

    // Read Domain
    if (data.find("domain") != data.end()) {
        result->m_domain = data.at("domain");
    } else {
        result->m_domain = result->m_curve_geometry->domain();
    }

    return result;
}

void BrepTrim::save(const Model& model, const BrepTrim& data, Json& target)
{
    target["loop"] = ToJson(data.m_loop);
    if (!data.m_edge.is_empty()) {
        target["edge"] = ToJson(data.m_edge);
    }
    target["geometry"] = ToJson(data.m_curve_geometry);
    target["domain"] = ToJson(data.m_domain);
}

// python

std::string BrepTrim::python_name()
{
    return "BrepTrim";
}

} // namespace anurbs