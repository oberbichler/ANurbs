#pragma once

#include "../Define.h"

#include "CurveTessellation.h"

#include "../Geometry/CurveBase.h"
#include "../Geometry/Interval.h"
#include "../Geometry/NurbsSurfaceGeometry.h"
#include "../Geometry/Polygon.h"
#include "../Geometry/SurfaceBase.h"

#include <ClipperLib/clipper.hpp>

#include <vector>

namespace anurbs {

enum TrimTypes {
    Empty,
    Full,
    trimmed
};

class TrimmedSurfaceClipping {
public: // types
    using Vector = Eigen::Matrix<double, 1, 2>;

private: // variables
    ClipperLib::Paths m_paths;
    std::vector<TrimTypes> m_span_trim_type;
    std::vector<std::vector<Polygon>> m_span_polygons;
    double m_scale;
    double m_tolerance;
    std::vector<Interval> m_spans_u;
    std::vector<Interval> m_spans_v;

    inline ClipperLib::IntPoint to_int_pt(const Vector& point) const
    {
        return to_int_pt(point[0], point[1]);
    }

    inline ClipperLib::IntPoint to_int_pt(const double x, const double y) const
    {
        ClipperLib::IntPoint int_pt;

        int_pt.X = (Index)(x / m_scale);
        int_pt.Y = (Index)(y / m_scale);

        return int_pt;
    }

    inline Vector from_int_pt(const ClipperLib::IntPoint& int_pt) const
    {
        Vector point;

        point[0] = int_pt.X * m_scale;
        point[1] = int_pt.Y * m_scale;

        return point;
    }

private: // static methods
    static inline bool is_rect(ClipperLib::Path rect, ClipperLib::Path contour,
        std::size_t a, std::size_t b, std::size_t c, std::size_t d)
    {
        if (contour[a] == rect[0] && contour[b] == rect[1] && contour[c] == rect[2] && contour[d] == rect[3]) {
            return true;
        } else {
            return false;
        }
    }

    static inline bool is_rect(ClipperLib::Path a, ClipperLib::Path b)
    {
        return is_rect(a, b, 0, 1, 2, 3) || is_rect(a, b, 1, 2, 3, 0) || is_rect(a, b, 2, 3, 0, 1) || is_rect(a, b, 3, 0, 1, 2) || is_rect(a, b, 3, 2, 1, 0) || is_rect(a, b, 0, 3, 2, 1) || is_rect(a, b, 1, 0, 3, 2) || is_rect(a, b, 2, 1, 0, 3);
    }

    void compute_span(const Index index_u, const Index index_v, const Interval span_u, const Interval span_v)
    {
        ClipperLib::Paths clip(1);
        ClipperLib::PolyTree polytree;

        clip[0] << to_int_pt(span_u.t0(), span_v.t0())
                << to_int_pt(span_u.t1(), span_v.t0())
                << to_int_pt(span_u.t1(), span_v.t1())
                << to_int_pt(span_u.t0(), span_v.t1());

        ClipperLib::Clipper clipper;
        clipper.AddPaths(m_paths, ClipperLib::ptSubject, true);
        clipper.AddPaths(clip, ClipperLib::ptClip, true);
        clipper.Execute(ClipperLib::ctIntersection, polytree,
            ClipperLib::pftNonZero, ClipperLib::pftNonZero);

        ClipperLib::PolyNode* polynode = polytree.GetFirst();

        std::vector<Polygon> regions;

        if (polytree.Total() == 0) {
            span_trim_type(index_u, index_v) = TrimTypes::Empty;
            span_polygons(index_u, index_v) = {};
            return;
        }

        if (polytree.Total() == 1) {
            const auto& contour = polynode->Contour;

            if (contour.size() == 4 && is_rect(clip[0], contour)) {
                span_trim_type(index_u, index_v) = TrimTypes::Full;
                span_polygons(index_u, index_v) = {};
                return;
            }
        }

        while (polynode) {
            if (!polynode->IsHole()) {
                const auto& outer_contour = polynode->Contour;
                const auto& inner_contours = polynode->Childs;

                Polygon region;

                region.outer_path.reserve(outer_contour.size());

                for (const auto& pt : outer_contour) {
                    region.outer_path.push_back(from_int_pt(pt));
                }

                for (std::size_t i = 0; i < inner_contours.size(); i++) {
                    typename Polygon::Path inner_path;

                    inner_path.reserve(inner_contours[i]->Contour.size());

                    for (const auto& pt : inner_contours[i]->Contour) {
                        inner_path.push_back(from_int_pt(pt));
                    }

                    region.inner_paths.push_back(inner_path);
                }

                regions.push_back(region);
            }

            polynode = polynode->GetNext();
        }

        span_trim_type(index_u, index_v) = TrimTypes::trimmed;
        span_polygons(index_u, index_v) = regions;
    }

public: // constructors
    TrimmedSurfaceClipping(const double tolerance, const double unit)
        : m_tolerance(tolerance)
        , m_scale(unit)
        , m_span_polygons(0)
        , m_span_trim_type(0)
    {
    }

public: // methods
    void clear()
    {
        m_paths.clear();
    }

    void begin_loop()
    {
        ClipperLib::Path path;
        m_paths.push_back(path);
    }

    void end_loop()
    {
        ClipperLib::CleanPolygon(m_paths.back());
    }

    void add_curve(CurveBase<2>& curve)
    {
        ClipperLib::Path& path = m_paths.back();

        const auto [ts, points] = CurveTessellation<2>::compute(curve, m_tolerance);

        for (Index i = 0; i < length(points); i++) {
            auto pt = to_int_pt(points[i]);

            if (i == 0 && length(path) > 0 && path.back() == pt) {
                continue;
            }

            path.push_back(pt);
        }
    }

    template <Index TDimension>
    void compute(NurbsSurfaceGeometry<TDimension>& surface)
    {
        compute(surface.SpansU(), surface.SpansV());
    }

    void compute(const std::vector<Interval>& spans_u,
        const std::vector<Interval>& spans_v)
    {
        m_spans_u = spans_u;
        m_spans_v = spans_v;

        m_span_trim_type.resize(nb_spans_u() * nb_spans_v());
        m_span_polygons.resize(nb_spans_u() * nb_spans_v());

        for (Index v = 0; v < nb_spans_v(); v++) {
            for (Index u = 0; u < nb_spans_u(); u++) {
                compute_span(u, v, span_u(u), span_v(v));
            }
        }
    }

    Index nb_spans_u() const
    {
        return static_cast<Index>(m_spans_u.size());
    }

    Index nb_spans_v() const
    {
        return static_cast<Index>(m_spans_v.size());
    }

    Interval span_u(const Index index) const
    {
        return m_spans_u[index];
    }

    Interval span_v(const Index index) const
    {
        return m_spans_v[index];
    }

    TrimTypes& span_trim_type(Index i, Index j)
    {
        return m_span_trim_type[i * nb_spans_v() + j];
    }

    TrimTypes span_trim_type(Index index_u, Index index_v) const
    {
        return span_trim_type(index_u, index_v);
    }

    std::vector<Polygon>& span_polygons(Index i, Index j)
    {
        return m_span_polygons[i * nb_spans_v() + j];
    }

    const std::vector<Polygon>& span_polygons(Index index_u, Index index_v) const
    {
        return span_polygons(index_u, index_v);
    }

public: // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        py::enum_<TrimTypes>(m, "TrimTypes")
            .value("Empty", TrimTypes::Empty)
            .value("Full", TrimTypes::Full)
            .value("trimmed", TrimTypes::trimmed)
            .export_values();

        using Type = TrimmedSurfaceClipping;

        py::class_<Type>(m, "TrimmedSurfaceClipping")
            .def(py::init<double, double>(), "tolerance"_a, "scale"_a)
            .def("clear", &Type::clear)
            .def("begin_loop", &Type::begin_loop)
            .def("end_loop", &Type::end_loop)
            .def("add_curve", &Type::add_curve, "curve"_a)
            .def("compute", (void (Type::*)(const std::vector<anurbs::Interval>&, const std::vector<anurbs::Interval>&)) & Type::compute,
                "knotsU"_a, "knotsV"_a)
            .def("nb_spans_u", &Type::nb_spans_u)
            .def("nb_spans_v", &Type::nb_spans_v)
            .def("span_u", &Type::span_u, "index"_a)
            .def("span_v", &Type::span_v, "index"_a)
            // .def("span_trim_type", &Type::span_trim_type, "index_u"_a, "index_v"_a)
            // .def("span_polygons", &Type::span_polygons, "index_u"_a, "index_v"_a)
            ;
    }
};

} // namespace anurbs