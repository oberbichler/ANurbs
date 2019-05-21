#pragma once

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace ANurbs {

class BrepTrim;

size_t BrepEdge::nb_trims()
{
    return m_trims.size();
}

Ref<BrepTrim> BrepEdge::trim(size_t index)
{
    return m_trims[index];
}

std::vector<Ref<BrepTrim>> BrepEdge::trims()
{
    return m_trims;
}

std::vector<Ref<BrepFace>> BrepEdge::faces()
{
    std::vector<Ref<BrepFace>> result(nb_trims());
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = trim(i)->loop()->face();
    }
    return result;
}

std::vector<Ref<BrepLoop>> BrepEdge::loops()
{
    std::vector<Ref<BrepLoop>> result(nb_trims());
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = trim(i)->loop();
    }
    return result;
}

} // namespace ANurbs