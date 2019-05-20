#pragma once

#include "Ref.h"
#include "Model.h"
#include "BrepEdge.h"

#include "Json.h"

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
    static std::string
    Type()
    {
        return "BrepTrim";
    }

    Ref<BrepLoop>
    Loop()
    {
        return m_loop;
    }

    Ref<BrepEdge>
    Edge()
    {
        return m_edge;
    }

    Ref<NurbsCurveGeometry<2>>
    Geometry()
    {
        return m_geometry;
    }

    Interval
    Domain()
    {
        return m_domain;
    }

    static Unique<BrepTrim>
    Load(
        Model& model,
        const Json& data)
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

    void
    Save(
        const Model& model,
        Json& data) const
    {
        data["Loop"] = ToJson(m_loop);
        if (!m_edge.IsEmpty()) {
            data["Edge"] = ToJson(m_edge);
        }
        data["Geometry"] = ToJson(m_geometry);
        data["Domain"] = ToJson(m_domain);
    }
};

} // namespace ANurbs