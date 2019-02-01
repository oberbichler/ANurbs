#pragma once

#include "BrepFace.h"
#include "BrepLoop.h"
#include "BrepTrim.h"
#include "BrepEdge.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <vector>

namespace ANurbs {

class Brep
{
private:
    std::vector<Ref<BrepFace>> m_faces;
    std::vector<Ref<BrepLoop>> m_loops;
    std::vector<Ref<BrepTrim>> m_trims;
    std::vector<Ref<BrepEdge>> m_edges;

public:
    static std::string
    Type()
    {
        return "Brep";
    }

    size_t
    NbFaces() const
    {
        return m_faces.size();
    }

    Ref<BrepFace>
    Face(
        size_t index) const
    {
        return m_faces[index];
    }

    std::vector<Ref<BrepFace>>
    Faces()
    {
        return m_faces;
    }

    size_t
    NbLoops() const
    {
        return m_loops.size();
    }

    Ref<BrepLoop>
    Loop(
        size_t index) const
    {
        return m_loops[index];
    }

    std::vector<Ref<BrepLoop>>
    Loops()
    {
        return m_loops;
    }

    size_t
    NbTrims() const
    {
        return m_trims.size();
    }

    Ref<BrepTrim>
    Trim(
        size_t index) const
    {
        return m_trims[index];
    }

    std::vector<Ref<BrepTrim>>
    Trims()
    {
        return m_trims;
    }

    size_t
    NbEdges() const
    {
        return m_edges.size();
    }

    Ref<BrepEdge>
    Edge(
        size_t index) const
    {
        return m_edges[index];
    }

    std::vector<Ref<BrepEdge>>
    Edges()
    {
        return m_edges;
    }

    static Unique<Brep>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<Brep>();

        // Read Faces
        {
            const auto faces = data.at("Faces");

            result->m_faces.resize(faces.size());

            for (size_t i = 0; i < faces.size(); i++) {
                const std::string key = faces[i];
                result->m_faces[i] = model.GetLazy<BrepFace>(key);
            }
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

        // Read Trims
        {
            const auto trims = data.at("Trims");

            result->m_trims.resize(trims.size());

            for (size_t i = 0; i < trims.size(); i++) {
                const std::string key = trims[i];
                result->m_trims[i] = model.GetLazy<BrepTrim>(key);
            }
        }

        // Read Edges
        {
            const auto edges = data.at("Edges");

            result->m_edges.resize(edges.size());

            for (size_t i = 0; i < edges.size(); i++) {
                const std::string key = edges[i];
                result->m_edges[i] = model.GetLazy<BrepEdge>(key);
            }
        }

        return result;
    }

    void
    Save(
        const Model& model,
        Json& data) const
    {
        data["Faces"] = ToJson(m_faces);
        data["Loops"] = ToJson(m_loops);
        data["Trims"] = ToJson(m_trims);
        data["Edges"] = ToJson(m_edges);
    }
};

} // namespace ANurbs