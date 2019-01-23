#pragma once

#include "Define.h"
#include "Ref.h"
#include "Model.h"
#include "BrepLoop.h"

#include "Json.h"

#include <vector>

namespace ANurbs {

class BrepFace
{
private:
    Ref<Brep> m_brep;
    std::vector<Ref<BrepLoop>> m_loops;
    Ref<SurfaceGeometry3D> m_geometry;

public:
    static std::string
    Type()
    {
        return "BrepFace";
    }

    Ref<Brep>
    Brep(
        size_t index)
    {
        return m_brep;
    }

    size_t
    NbLoops()
    {
        return m_loops.size();
    }

    Ref<BrepLoop>
    Loop(
        size_t index)
    {
        return m_loops[index];
    }

    static Unique<BrepFace>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<BrepFace>();

        // Read Brep
        {
            const std::string key = data.at("Brep");
            result->m_brep = model.GetLazy<ANurbs::Brep>(key);
        }

        // Read Loops
        {
            const auto loops = data.at("Loops");

            result->m_loops.resize(loops.size());

            for (size_t i = 0; i < loops.size(); i++) {
                const std::string key = loops[i];
                result->m_loops[i] = model.GetLazy<BrepLoop>(key);
            }
        }

        // Read Geometry
        {
            const std::string key = data.at("Geometry");
            result->m_geometry = model.GetLazy<SurfaceGeometry3D>(key);
        }

        return result;
    }

    void
    Save(
        const Model& model,
        Json& data) const
    {
        data["Brep"] = ToJson(m_brep);
        data["Loops"] = ToJson(m_loops);
        data["Geometry"] = ToJson(m_geometry);
    }
};

} // namespace ANurbs