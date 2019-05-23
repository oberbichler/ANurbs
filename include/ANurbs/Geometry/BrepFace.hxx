#pragma once

#include "BrepLoop.h"
#include "NurbsSurfaceGeometry.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

Ref<ANurbs::Brep> BrepFace::brep() const
{
    return m_brep;
}

size_t BrepFace::nb_loops() const
{
    return m_loops.size();
}

Ref<BrepLoop> BrepFace::loop(size_t index) const
{
    return m_loops[index];
}

std::vector<Ref<BrepLoop>> BrepFace::loops() const
{
    return m_loops;
}

Ref<NurbsSurfaceGeometry<3>> BrepFace::surface_geometry() const
{
    return m_geometry;
}

std::vector<Ref<BrepEdge>> BrepFace::edges() const
{
    std::vector<Ref<BrepEdge>> result;
    for (size_t i = 0; i < nb_loops(); i++) {
        auto loop = this->loop(i);
        for (size_t j = 0; j < loop->nb_trims(); j++) {
            result.push_back(loop->trim(j)->edge());
        }
    }
    return result;
}

std::vector<Ref<BrepTrim>> BrepFace::trims() const
{
    std::vector<Ref<BrepTrim>> result;
    for (size_t i = 0; i < nb_loops(); i++) {
        auto loop = this->loop(i);
        for (size_t j = 0; j < loop->nb_trims(); j++) {
            result.push_back(loop->trim(j));
        }
    }
    return result;
}

} // namespace ANurbs