#pragma once

#include "../Define"
#include "../Geometry/Box.h"

#include <vector>

namespace ANurbs {

template <Index TDimension>
class RTree
{
private:    // types
    using Vector = linear_algebra::Vector<TDimension>;

public:     // constructor
    RTree(const std::vector<Pointer<Box>> items)
    {
    }
};

} // namespace ANurbs