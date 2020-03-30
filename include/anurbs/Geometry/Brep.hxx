#pragma once

#include "BrepFace.h"
#include "BrepLoop.h"
#include "BrepTrim.h"
#include "BrepEdge.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace anurbs {

size_t Brep::nb_faces() const
{
    return m_faces.size();
}

Ref<BrepFace> Brep::face(size_t index) const
{
    return m_faces[index];
}

std::vector<Ref<BrepFace>> Brep::faces()
{
    return m_faces;
}

size_t Brep::nb_loops() const
{
    return m_loops.size();
}

Ref<BrepLoop> Brep::loop(size_t index) const
{
    return m_loops[index];
}

std::vector<Ref<BrepLoop>> Brep::loops()
{
    return m_loops;
}

size_t Brep::nb_trims() const
{
    return m_trims.size();
}

Ref<BrepTrim> Brep::trim(size_t index) const
{
    return m_trims[index];
}

std::vector<Ref<BrepTrim>> Brep::trims()
{
    return m_trims;
}

size_t Brep::nb_edges() const
{
    return m_edges.size();
}

Ref<BrepEdge> Brep::edge(size_t index) const
{
    return m_edges[index];
}

std::vector<Ref<BrepEdge>> Brep::edges()
{
    return m_edges;
}

std::string Brep::type_name()
{
    return "Brep";
}

Unique<Brep> Brep::load(Model& model, const Json& data)
{
    auto result = new_<Brep>();

    // Read faces
    {
        const auto faces = data.at("faces");

        result->m_faces.resize(faces.size());

        for (size_t i = 0; i < faces.size(); i++) {
            const std::string key = faces[i];
            result->m_faces[i] = model.get_lazy<BrepFace>(key);
        }
    }

    // Read loops
    {
        const auto loops = data.at("loops");

        result->m_loops.resize(loops.size());

        for (size_t i = 0; i < loops.size(); i++) {
            const std::string key = loops[i];
            result->m_loops[i] = model.get_lazy<BrepLoop>(key);
        }
    }

    // Read trims
    {
        const auto trims = data.at("trims");

        result->m_trims.resize(trims.size());

        for (size_t i = 0; i < trims.size(); i++) {
            const std::string key = trims[i];
            result->m_trims[i] = model.get_lazy<BrepTrim>(key);
        }
    }

    // Read edges
    {
        const auto edges = data.at("edges");

        result->m_edges.resize(edges.size());

        for (size_t i = 0; i < edges.size(); i++) {
            const std::string key = edges[i];
            result->m_edges[i] = model.get_lazy<BrepEdge>(key);
        }
    }

    return result;
}

void Brep::save(const Model& model, const Brep& data, Json& target)
{
    target["faces"] = ToJson(data.m_faces);
    target["loops"] = ToJson(data.m_loops);
    target["trims"] = ToJson(data.m_trims);
    target["edges"] = ToJson(data.m_edges);
}

// python

std::string Brep::python_name()
{
    return "Brep";
}

} // namespace anurbs