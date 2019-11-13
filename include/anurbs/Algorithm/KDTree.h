#pragma once

#include "../Define.h"

#include <nanoflann/nanoflann.hpp>

#include <utility>

namespace anurbs {

template <int TDimension>
class KDTree
{
private:    // types
    using Callback = std::function<bool(Index)>;
    using Vector = linear_algebra::Vector<TDimension>;
    using Points = std::vector<Vector>;
    using Type = RTree<TDimension>;
    
    struct Adaptor
    {
        const std::vector<Points>& m_points;

        Adaptor(const std::vector<Points>& points) : m_points(points)
        { 
        }

        inline size_t kdtree_get_point_count() const
        { 
            return m_points.size();
        }

        inline double kdtree_get_pt(const size_t idx, const size_t dim) const
        {
            return m_points[idx].point[dim];
        }

        template <typename BBOX>
        bool kdtree_get_bbox(BBOX&) const
        { 
            return false;
        }
    };

private:    // variables
    Points m_points;
    Adaptor m_adaptor;

public:     // constructors
    KDTree(const Points& points)
    : m_points(points)
    , m_adaptor(m_points)
    {
        m_adaptor.index->buildIndex();
    }

public:     // methods
    const Point& point(index index) const
    {
        return m_points.rows(index);
    }

    std::pair<index, double> search(const Point& point) const
    {
        const size_t num_results = 1;
        size_t index;
        double distance;
        
        nanoflann::KNNResultSet<double> result_set(num_results);
        result_set.init(&index, &distance);
        
        m_adaptor.index->findNeighbors(result_set, &point[0],
            nanoflann::SearchParams(1));
        
        return {index, distance};
    }

    std::pair<std::vector<index>, std::vector<double>> search(const std::vector<Point>& points) const
    {
        std::vector<index> indices(points.size());
        std::vector<double> distances(points.size());

        #pragma omp parallel for
        for (index i = 0; i < length(points); i++) {
            const auto [index, distance] = search(points[i]);
            indices[i] = index;
            distances[i] = distance;
        }
        
        return {indices, distances};
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = KDTree<TDimension>;

        const std::string name = "KDTree" + std::to_string(TDimension) + "D";

        py::class_<Type>(m, name.c_str())
            // constructors
            .def(py::init<Points>(), "points"_a)
            // methods
            .def("search", py::overload_cast<const Point&>(&Type::search, py::const_), "point"_a)
            .def("search", py::overload_cast<const std::vector<Point>&>(&Type::search, py::const_), "points"_a)
        ;
    }
};

} // namespace anurbs