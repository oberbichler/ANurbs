#pragma once

#include "BrepTrim.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

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

} // namespace ANurbs