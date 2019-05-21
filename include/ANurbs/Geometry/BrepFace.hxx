#pragma once

#include "BrepLoop.h"
#include "NurbsSurfaceGeometry.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

    Ref<ANurbs::Brep> BrepFace::Brep()
    {
        return m_brep;
    }

    size_t BrepFace::nb_loops()
    {
        return m_loops.size();
    }

    Ref<BrepLoop> BrepFace::loop(size_t index)
    {
        return m_loops[index];
    }

    std::vector<Ref<BrepLoop>> BrepFace::loops()
    {
        return m_loops;
    }
    
    Ref<NurbsSurfaceGeometry<3>> BrepFace::Geometry()
    {
        return m_geometry;
    }
} // namespace ANurbs