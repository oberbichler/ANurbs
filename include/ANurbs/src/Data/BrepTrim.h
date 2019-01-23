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
    Ref<CurveGeometry2D> m_geometry;

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

    Ref<CurveGeometry2D>
    Geometry()
    {
        return m_geometry;
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
        {
            const std::string key = data.at("Edge");
            result->m_edge = model.GetLazy<BrepEdge>(key);
        }

        // Read Geometry
        {
            const std::string key = data.at("Geometry");
            result->m_geometry = model.GetLazy<CurveGeometry2D>(key);
        }

        return result;
    }

    void
    Save(
        const Model& model,
        Json& data) const
    {
        data["Loop"] = ToJson(m_loop);
        data["Edge"] = ToJson(m_edge);
        data["Geometry"] = ToJson(m_geometry);
    }
};

} // namespace ANurbs