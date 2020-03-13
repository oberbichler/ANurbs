#pragma once

#include "BrepEdge.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace anurbs {

class BrepTrim;

size_t BrepEdge::nb_trims() const
{
    return m_trims.size();
}

Ref<BrepTrim> BrepEdge::trim(size_t index) const
{
    return m_trims[index];
}

std::vector<Ref<BrepTrim>> BrepEdge::trims() const
{
    return m_trims;
}

std::vector<Ref<BrepFace>> BrepEdge::faces() const
{
    std::vector<Ref<BrepFace>> result(nb_trims());
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = trim(i)->loop()->face();
    }
    return result;
}

std::vector<Ref<BrepLoop>> BrepEdge::loops() const
{
    std::vector<Ref<BrepLoop>> result(nb_trims());
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = trim(i)->loop();
    }
    return result;
}

// serialization

std::string BrepEdge::type_name()
{
    return "BrepEdge";
}

Unique<BrepEdge> BrepEdge::load(Model& model, const Json& data)
{
    auto result = new_<BrepEdge>();

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

void BrepEdge::save(const Model& model, const BrepEdge& data, Json& target)
{
    target["trims"] = ToJson(data.m_trims);
}

// python

std::string BrepEdge::python_name()
{
    return "BrepEdge";
}

} // namespace anurbs