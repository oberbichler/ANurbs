#pragma once

#include "BrepTrim.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace anurbs {

Ref<Brep> BrepLoop::brep() const
{
    return m_face->brep();
}

Ref<BrepFace> BrepLoop::face() const
{
    return m_face;
}

size_t BrepLoop::nb_trims() const
{
    return m_trims.size();
}

Ref<BrepTrim> BrepLoop::trim(size_t index) const
{
    return m_trims[index];
}

std::vector<Ref<BrepTrim>> BrepLoop::trims() const
{
    return m_trims;
}

std::vector<Ref<BrepEdge>> BrepLoop::edges() const
{
    std::vector<Ref<BrepEdge>> result(nb_trims());

    for (size_t i = 0; i < result.size(); i++) {
        result[i] = trim(i)->edge();
    }

    return result;
}

// serialization

std::string BrepLoop::type_name()
{
    return "BrepLoop";
}

Unique<BrepLoop> BrepLoop::load(Model& model, const Json& data)
{
    auto result = new_<BrepLoop>();

    // Read Face
    {
        const std::string key = data.at("face");
        result->m_face = model.get_lazy<BrepFace>(key);
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

    return result;
}

void BrepLoop::save(const Model& model, const BrepLoop& data, Json& target)
{
    target["face"] = ToJson(data.m_face);
    target["trims"] = ToJson(data.m_trims);
}

// python

std::string BrepLoop::python_name()
{
    return "BrepLoop";
}

} // namespace anurbs