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

} // namespace ANurbs