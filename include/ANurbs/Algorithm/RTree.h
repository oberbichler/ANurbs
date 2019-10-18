#pragma once

#include "../Define.h"

#include "HilbertCurve.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <stdexcept>
#include <vector>

namespace ANurbs {

template <Index TDimension>
class RTree
{
    // Sources:
    // 
    // - 'Flatbush'
    //    by Vladimir Agafonkin
    //    https://github.com/mourner/flatbush

private:    // types
    using Callback = std::function<bool(Index)>;
    using Vector = linear_algebra::Vector<TDimension>;
    using VectorU = Eigen::Matrix<size_t, 1, TDimension>;
    using Type = RTree<TDimension>;

    template <bool TIntersection=true>
    struct ContainsBox
    {
        Vector m_box_min;
        Vector m_box_max;

        ContainsBox(const Vector box_a, const Vector box_b) noexcept
        {
            for (Index i = 0; i < TDimension; i++) {
                if (box_a[i] < box_b[i]) {
                    m_box_min[i] = box_a[i];
                    m_box_max[i] = box_b[i];
                } else {
                    m_box_min[i] = box_b[i];
                    m_box_max[i] = box_a[i];
                }
            }
        }

        bool operator()(const Vector node_min, const Vector node_max) const noexcept
        {
            if (TIntersection) {
                for (Index i = 0; i < TDimension; i++) {
                    if (m_box_max[i] < node_min[i]) {
                        return false;
                    }
                    if (m_box_min[i] > node_max[i]) {
                        return false;
                    }
                }
            } else {
                for (Index i = 0; i < TDimension; i++) {
                    if (m_box_min[i] < node_min[i]) {
                        return false;
                    }
                    if (m_box_max[i] > node_max[i]) {
                        return false;
                    }
                }
            }
            return true;
        }
    };

    struct IntersectsRay
    {
        Vector m_origin;
        Vector m_direction;

        IntersectsRay(const Vector origin, const Vector direction) noexcept
        : m_origin(origin), m_direction(direction)
        {
        }

        bool operator()(const Vector node_min, const Vector node_max) const noexcept
        {
            // based on Fast Ray-Box Intersection
            // by Andrew Woo
            // from "Graphics Gems", Academic Press, 1990

            bool inside = true;
            VectorU quadrant = VectorU::Zero(); // FIXME: choose other type...
            int which_plane = 0;
            Vector max_t = Vector::Zero();
            Vector candidate_plane = Vector::Zero();
            Vector coordinate = Vector::Zero();

            for (Index i = 0; i < TDimension; i++) {
                if (m_origin[i] < node_min[i]) {
                    quadrant[i] = -1;
                    candidate_plane[i] = node_min[i];
                    inside = false;
                } else if (m_origin[i] > node_max[i]) {
                    quadrant[i] = 1;
                    candidate_plane[i] = node_max[i];
                    inside = false;
                } else {
                    quadrant[i] = 0;
                }
            }

            if (inside) {
                coordinate = m_origin;
                return true;
            }

            for (Index i = 0; i < TDimension; i++) {
                if (quadrant[i] != 0 && m_direction[i] != 0) {
                    max_t[i] = (candidate_plane[i] - m_origin[i]) / m_direction[i];
                } else {
                    max_t[i] = -1;
                }
            }

            which_plane = 0;
            
            for (Index i = 1; i < TDimension; i++) {
                if (max_t[which_plane] < max_t[i]) {
                    which_plane = i;
                }
            }

            if (max_t[which_plane] < 0) {
                return false;
            }

            for (Index i = 0; i < TDimension; i++) {
                if (which_plane != i) {
                    coordinate[i] = m_origin[i] + max_t[which_plane] * m_direction[i];
                    if (coordinate[i] < node_min[i] or coordinate[i] > node_max[i]) {
                        return false;
                    }
                } else {
                    coordinate[i] = candidate_plane[i];
                }
            }

            return true;
        }
    };

private:    // variables
    Index m_nb_items;
    Index m_node_size;
    std::vector<Index> m_level_bounds;
    Vector m_min;
    Vector m_max;
    Index m_position;
    std::vector<Index> m_indices;
    std::vector<Vector> m_boxes_min;
    std::vector<Vector> m_boxes_max;

public:     // static methods
    static constexpr Index dimension()
    {
        return TDimension;
    }

public:     // constructor
    RTree(const Index nb_items, const Index node_size)
    {
        if (nb_items < 0) {
            throw std::invalid_argument("nb_items");
        }

        m_nb_items = nb_items;
        m_node_size = std::min<Index>(std::max<Index>(node_size, 2), ((size_t)1 << (4 * sizeof(size_t))) - 1);

        Index n = nb_items;
        Index nb_nodes = n;

        m_level_bounds.emplace_back(n);

        do {
            n = (Index)(std::ceil((double)n / m_node_size));
            nb_nodes += n;
            m_level_bounds.emplace_back(nb_nodes);
        } while (n > 1);

        for (Index i = 0; i < TDimension; i++) {
            m_min[i] = std::numeric_limits<double>::infinity();
            m_max[i] = -std::numeric_limits<double>::infinity();
        }

        m_position = 0;
        m_indices.resize(nb_nodes);
        m_boxes_min.resize(nb_nodes);
        m_boxes_max.resize(nb_nodes);
    }

private:    // methods
    void sort(std::vector<size_t>& values, const Index left, const Index right)
    {
        if (left >= right) {
            return;
        }

        const size_t pivot = values[(left + right) >> 1];

        Index i = left - 1;
        Index j = right + 1;

        while (true) {
            do {
                i += 1;
            } while(values[i] < pivot);

            do {
                j -= 1;
            } while(values[j] > pivot);

            if (i >= j) {
                break;
            }

            swap(values, i, j);
        }

        sort(values, left, j);
        sort(values, j + 1, right);
    }

    void swap(std::vector<size_t>& values, const Index i, const Index j)
    {
        std::swap(values[i], values[j]);
        std::swap(m_indices[i], m_indices[j]);
        std::swap(m_boxes_min[i], m_boxes_min[j]);
        std::swap(m_boxes_max[i], m_boxes_max[j]);
    }

    const std::vector<Index>& level_bounds() const
    {
        return m_level_bounds;
    }

    Index position() const
    {
        return m_position;
    }

    const std::vector<Vector>& boxes_min() const
    {
        return m_boxes_min;
    }

    const std::vector<Vector>& boxes_max() const
    {
        return m_boxes_max;
    }

public:     // methods
    void add(const Vector box_a, const Vector box_b)
    {
        // FIXME: check m_position

        Index index = m_position++;

        m_indices[index] = index;

        Vector box_min;
        Vector box_max;

        for (Index i = 0; i < dimension(); i++) {
            if (box_a[i] < box_b[i]) {
                box_min[i] = box_a[i];
                box_max[i] = box_b[i];
            } else {
                box_min[i] = box_b[i];
                box_max[i] = box_a[i];
            }
        }

        m_boxes_min[index] = box_min;
        m_boxes_max[index] = box_max;

        for (Index i = 0; i < dimension(); i++) {
            if (box_min[i] < m_min[i]) {
                m_min[i] = box_min[i];
            }
            if (box_max[i] > m_max[i]) {
                m_max[i] = box_max[i];
            }
        }
    }

    void finish()
    {
        if (m_position != m_nb_items) {
            throw std::runtime_error("More items then expected");
        }

        const Vector size = m_max - m_min;

        std::vector<size_t> hilbert_values(m_nb_items);

        for (Index i = 0; i < m_nb_items; i++) {
            Vector box_min = m_boxes_min[i];
            Vector box_max = m_boxes_max[i];

            VectorU center;

            for (Index j = 0; j < dimension(); j++) {
                center[j] = ((box_min[j] + box_max[j]) / 2 - m_min[j]) / size[j] * HilbertCurve<TDimension>::max_axis_size();
            }

            hilbert_values[i] = HilbertCurve<TDimension>::index_at(center);
        }

        sort(hilbert_values, 0, m_nb_items - 1);

        Index pos = 0;

        for (Index i = 0; i < length(m_level_bounds) - 1; i++) {
            Index end = m_level_bounds[i];

            while (pos < end) {
                Vector node_min;
                Vector node_max;

                for (Index j = 0; j < TDimension; j++) {
                    node_min[j] = std::numeric_limits<double>::infinity();
                    node_max[j] = -std::numeric_limits<double>::infinity();
                }

                Index node_index = pos;

                for (Index j = 0; j < m_node_size && pos < end; j++) {
                    Vector box_min = m_boxes_min[pos];
                    Vector box_max = m_boxes_max[pos];

                    pos += 1;

                    for (Index k = 0; k < dimension(); k++) {
                        if (box_min[k] < node_min[k]) {
                            node_min[k] = box_min[k];
                        }
                        if (box_max[k] > node_max[k]) {
                            node_max[k] = box_max[k];
                        }
                    }
                }

                m_indices[m_position] = node_index;
                m_boxes_min[m_position] = node_min;
                m_boxes_max[m_position] = node_max;

                m_position += 1;
            }
        }
    }

    const std::vector<Index>& indices() const
    {
        return m_indices;
    }

    Vector min() const
    {
        return m_min;
    }

    Vector max() const
    {
        return m_max;
    }

    Index nb_items() const
    {
        return m_nb_items;
    }

    Index node_size() const
    {
        return m_node_size;
    }

    template <typename TCheck>
    std::vector<Index> search_for(const TCheck& check, Callback callback)
    {
        if (m_position != length(m_indices)) {
            throw std::runtime_error("Data not yet indexed - call RTree::finish().");
        }

        Index node_index = length(m_indices) - 1;
        Index level = length(m_level_bounds) - 1;
        std::vector<Index> queue;
        std::vector<Index> results;

        while (node_index > -1) {
            const Index end = std::min<Index>(node_index + m_node_size, m_level_bounds[level]);

            for (Index pos = node_index; pos < end; pos++) {
                const Index index = m_indices[pos];

                const Vector node_min = m_boxes_min[pos];
                const Vector node_max = m_boxes_max[pos];

                if (!check(node_min, node_max)) {
                    continue;
                }

                if (node_index < m_nb_items) {
                    if (callback == nullptr || callback(index)) {
                        results.push_back(index);
                    }
                } else {
                    queue.push_back(index);
                    queue.push_back(level - 1);
                }
            }

            if (queue.empty()) {
                node_index = -1;
                level = -1;
            } else {
                level = queue.back();
                queue.pop_back();
                node_index = queue.back();
                queue.pop_back();
            }
        }

        return results;
    }

    std::vector<Index> search(const Vector box_a, const Vector box_b, bool intersection, Callback callback)
    {
        if (intersection) {
            ContainsBox<true> check(box_a, box_b);
            return search_for(check, callback);
        } else {
            ContainsBox<false> check(box_a, box_b);
            return search_for(check, callback);
        }
    }

    std::vector<Index> search_ray_intersection(const Vector origin, const Vector direction, Callback callback)
    {
        IntersectsRay check(origin, direction);
        return search_for(check, callback);
    }

public:     // serialization
    static std::string type_name()
    {
        return "RTree" + std::to_string(dimension()) + "D";
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        std::string name = Type::type_name();

        py::class_<Type>(m, name.c_str())
            // constructors
            .def(py::init<Index, Index>(), "nb_items"_a, "node_size"_a=16)
            // private read-only properties
            .def_property_readonly("_boxes_min", &Type::boxes_min)
            .def_property_readonly("_boxes_max", &Type::boxes_max)
            // read-only properties
            .def_property_readonly("indices", &Type::indices)
            .def_property_readonly("min", &Type::min)
            .def_property_readonly("max", &Type::min)
            .def_property_readonly("nb_items", &Type::nb_items)
            .def_property_readonly("node_size", &Type::node_size)
            // methods
            .def("add", &Type::add, "box_a"_a, "box_b"_a)
            .def("finish", &Type::finish)
            .def("search", &Type::search, "box_a"_a, "box_b"_a, "intersection"_a=true, "callback"_a=py::none())
            .def("search_ray_intersection", &Type::search_ray_intersection, "origin"_a, "direction"_a, "callback"_a=py::none())
        ;
    }
};

} // namespace ANurbs