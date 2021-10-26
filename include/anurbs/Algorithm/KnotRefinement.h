#pragma once

#include "../Define.h"

#include "Nurbs.h"

#include "../Geometry/NurbsCurveGeometry.h"
#include "../Geometry/NurbsSurfaceGeometry.h"

#include <algorithm>
#include <vector>

namespace anurbs {

template <Index TDimension>
class KnotRefinement
{
public:     // types
    using CurveGeometry = NurbsCurveGeometry<TDimension>;
    using SurfaceGeometry = NurbsSurfaceGeometry<TDimension>;

public:     // static methods
    static Pointer<CurveGeometry> insert_knots(
        const CurveGeometry& geometry,
        std::vector<double> knots)
    {
        std::sort(knots.begin(), knots.end());

        const Index nb_knots_to_insert = static_cast<Index>(knots.size());

        const Index degree = geometry.degree();

        const Index nb_poles = geometry.nb_poles();

        const Index nb_knots = geometry.nb_knots();

        const Index a = Nurbs::upper_span(degree, geometry.knots(), knots.front());
        const Index b = Nurbs::upper_span(degree, geometry.knots(), knots.back());

        const Index nb_poles_refined = nb_poles + nb_knots_to_insert;
        const Index nb_knots_refined = nb_knots + nb_knots_to_insert + 2;

        Pointer<CurveGeometry> refined = new_<CurveGeometry>(degree,
            nb_poles_refined, true); // FIXME: check is_rational

        for (Index i = 0; i < a + 1 - degree + 1; i++) {
            refined->pole(i) = geometry.pole(i) * geometry.weight(i);
            refined->weight(i) = geometry.weight(i);
        }

        for (Index i = b + 2 - 1; i < nb_poles; i++) {
            refined->pole(nb_knots_to_insert + i) = geometry.pole(i) * geometry.weight(i);
            refined->weight(nb_knots_to_insert + i) = geometry.weight(i);
        }

        for (Index i = 0; i < a + 1; i++) {
            refined->knot(i) = geometry.knot(i);
        }

        for (Index i = b + degree + 1; i < nb_poles + degree - 2; i++) {
            refined->knot(i + nb_knots_to_insert) = geometry.knot(i);
        }

        const Index n = nb_poles - 1;
        const Index m = n + degree + 1;
        const Index r = nb_knots_to_insert - 1;

        Index i = b + degree + 1;
        Index k = b + degree + r + 2;
        Index j = r;

        while (j >= 0) {
            while (knots[j] <= geometry.knot(-1 + i) && i > a + 1) {
                const auto pole = geometry.pole(i - degree - 1);
                const auto weight = geometry.weight(i - degree - 1);
                refined->pole(k - degree - 1) = pole * weight;
                refined->weight(k - degree - 1) = weight;

                refined->knot(-1+k) = geometry.knot(-1 + i);

                k -= 1;
                i -= 1;
            }

            refined->pole(k - degree - 1) = refined->pole(k - degree);
            refined->weight(k - degree - 1) = refined->weight(k - degree);

            for (Index l = 1; l < degree + 1; l++) {
                const Index index = k - degree + l;
                auto alpha = refined->knot(-1+k + l) - knots[j];

                if (std::abs(alpha) < 1e-7) {
                    refined->pole(index - 1) = refined->pole(index);
                    refined->weight(index - 1) = refined->weight(index);
                } else {
                    alpha = alpha / (refined->knot(k + l - 1) - geometry.knot(i + l - degree - 1));
                    refined->pole(index - 1) = refined->pole(index - 1) * alpha + refined->pole(index) * (1 - alpha);
                    refined->weight(index - 1) = refined->weight(index - 1) * alpha + refined->weight(index) * (1 - alpha);
                }
            }

            refined->knot(-1 + k) = knots[j];

            k -= 1;
            j -= 1;
        }

        for (Index i = 0; i < refined->nb_poles(); i++) {
            refined->pole(i) = refined->pole(i) / refined->weight(i);
        }

        return refined;
    }

    static Pointer<SurfaceGeometry>
    insert_knots_u(const SurfaceGeometry& geometry, std::vector<double> knotsU)
    {
        std::sort(knotsU.begin(), knotsU.end());

        const Index nb_knots_to_insert = static_cast<Index>(knotsU.size());

        const Index degree_u = geometry.degree_u();
        const Index degree_v = geometry.degree_v();

        const Index nb_poles_u = geometry.nb_poles_u();
        const Index nb_poles_v = geometry.nb_poles_v();

        const Index nb_knots_u = geometry.nb_knots_u();
        const Index nb_knots_v = geometry.nb_knots_v();

        const Index a = Nurbs::upper_span(degree_u, geometry.knots_u(), knotsU.front());
        const Index b = Nurbs::upper_span(degree_u, geometry.knots_u(), knotsU.back());

        const Index nb_poles_refined = nb_poles_u + nb_knots_to_insert;
        const Index nb_knots_refined = geometry.nb_knots_u() + 2 + nb_knots_to_insert;

        Pointer<SurfaceGeometry> refined = new_<SurfaceGeometry>(degree_u,
            degree_v, nb_poles_refined, nb_poles_v, true); // FIXME: check is_rational

        for (Index i = 0; i < a + 1 - degree_u + 1; i++) {
            for (Index m = 0; m < geometry.nb_poles_v(); m++) {
                refined->pole(i, m) = geometry.pole(i, m) * geometry.weight(i, m);
                refined->weight(i, m) = geometry.weight(i, m);
            }
        }

        for (Index i = b + 2 - 1; i < nb_poles_u; i++) {
            for (Index m = 0; m < geometry.nb_poles_v(); m++) {
                refined->pole(nb_knots_to_insert + i, m) = geometry.pole(i, m) * geometry.weight(i, m);
                refined->weight(nb_knots_to_insert + i, m) = geometry.weight(i, m);
            }
        }

        for (Index i = 0; i < a + 1; i++) {
            refined->knot_u(i) = geometry.knot_u(i);
        }

        for (Index i = b + 2 + degree_u - 1; i < geometry.nb_knots_u(); i++) {
            refined->knot_u(i + nb_knots_to_insert) = geometry.knot_u(i);
        }

        for (Index i = 0; i < geometry.nb_knots_v(); i++) {
            refined->knot_v(i) = geometry.knot_v(i);
        }

        const Index n = nb_poles_u - 1;
        const Index m = n + degree_u + 1;
        const Index r = nb_knots_to_insert - 1;

        Index i = b + 2 + degree_u - 1;
        Index k = b + 2 + degree_u + r;
        Index j = r;

        while (j >= 0) {
            while (knotsU[j] <= geometry.knot_u(-1 + i) && i > a + 1) {
                for (Index m = 0; m < geometry.nb_poles_v(); m++) {
                    const auto pole = geometry.pole(i - degree_u - 1, m);
                    const auto weight = geometry.weight(i - degree_u - 1, m);
                    refined->pole(k - degree_u-1, m) = pole * weight;
                    refined->weight(k - degree_u-1, m) = weight;
                }

                refined->knot_u(-1+k) = geometry.knot_u(-1 + i);

                k -= 1;
                i -= 1;
            }

            for (Index m = 0; m < geometry.nb_poles_v(); m++) {
                refined->pole(k - degree_u-1, m) = refined->pole(k - degree_u, m);
                refined->weight(k - degree_u-1, m) = refined->weight(k - degree_u, m);
            }

            for (Index l = 1; l < degree_u + 1; l++) {
                const Index index = k - degree_u + l;
                auto alpha = refined->knot_u(-1+k + l) - knotsU[j];

                if (std::abs(alpha) < 1e-7) {
                    for (Index m = 0; m < geometry.nb_poles_v(); m++) {
                        refined->pole(index - 1, m) = refined->pole(index, m);
                        refined->weight(index - 1, m) = refined->weight(index, m);
                    }
                } else {
                    alpha = alpha / (refined->knot_u(k + l - 1) - geometry.knot_u(i + l - degree_u - 1));
                    for (Index m = 0; m < geometry.nb_poles_v(); m++) {
                        refined->pole(index - 1, m) = refined->pole(index - 1, m) * alpha + refined->pole(index, m) * (1 - alpha);
                        refined->weight(index - 1, m) = refined->weight(index - 1, m) * alpha + refined->weight(index, m) * (1 - alpha);
                    }
                }
            }

            refined->knot_u(-1 + k) = knotsU[j];

            k -= 1;
            j -= 1;
        }

        for (Index i = 0; i < refined->nb_poles(); i++) {
            refined->pole(i) = refined->pole(i) / refined->weight(i);
        }

        return refined;
    }

    static Pointer<SurfaceGeometry> insert_knots_v(
        const SurfaceGeometry& geometry,
        std::vector<double> knotsV)
    {
        std::sort(knotsV.begin(), knotsV.end());

        const Index nb_knots_to_insert = static_cast<Index>(knotsV.size());

        const Index degree_u = geometry.degree_u();
        const Index degree_v = geometry.degree_v();

        const Index nb_poles_u = geometry.nb_poles_u();
        const Index nb_poles_v = geometry.nb_poles_v();

        const Index nb_knots_u = geometry.nb_knots_u();
        const Index nb_knots_v = geometry.nb_knots_v();

        const Index a = Nurbs::upper_span(degree_v, geometry.knots_v(),
            knotsV.front());
        const Index b = Nurbs::upper_span(degree_v, geometry.knots_v(),
            knotsV.back());

        const Index nb_poles_refined = nb_poles_v + nb_knots_to_insert;
        const Index nb_knots_refined = nb_knots_v + 2 + nb_knots_to_insert;

        Pointer<SurfaceGeometry> refined = new_<SurfaceGeometry>(degree_u,
            degree_v, nb_poles_u, nb_poles_refined, true); // FIXME: check is_rational

        for (Index i = 0; i < a + 1 - degree_v + 1; i++) {
            for (Index m = 0; m < geometry.nb_poles_u(); m++) {
                refined->pole(m, i) = geometry.pole(m, i) * geometry.weight(m, i);
                refined->weight(m, i) = geometry.weight(m, i);
            }
        }

        for (Index i = b + 2 - 1; i < nb_poles_v; i++) {
            for (Index m = 0; m < geometry.nb_poles_u(); m++) {
                refined->pole(m, nb_knots_to_insert + i) = geometry.pole(m, i) * geometry.weight(m, i);
                refined->weight(m, nb_knots_to_insert + i) = geometry.weight(m, i);
            }
        }

        for (Index i = 0; i < a + 1; i++) {
            refined->knot_v(i) = geometry.knot_v(i);
        }

        for (Index i = b + 2 + degree_v - 1; i < geometry.nb_knots_v(); i++) {
            refined->knot_v(i + nb_knots_to_insert) = geometry.knot_v(i);
        }

        for (Index i = 0; i < geometry.nb_knots_u(); i++) {
            refined->knot_u(i) = geometry.knot_u(i);
        }

        const Index n = nb_poles_v - 1;
        const Index m = n + degree_v + 1;
        const Index r = nb_knots_to_insert - 1;

        Index i = b + 2 + degree_v - 1;
        Index k = b + 2 + degree_v + r;
        Index j = r;

        while (j >= 0) {
            while (knotsV[j] <= geometry.knot_v(-1 + i) && i > a + 1) {
                for (Index m = 0; m < geometry.nb_poles_u(); m++) {
                    const auto pole = geometry.pole(m, i - degree_v - 1);
                    const auto weight = geometry.weight(m, i - degree_v - 1);
                    refined->pole(m, k - degree_v - 1) = pole * weight;
                    refined->weight(m, k - degree_v - 1) = weight;
                }

                refined->knot_v(-1 + k) = geometry.knot_v(-1 + i);

                k -= 1;
                i -= 1;
            }

            for (Index m = 0; m < geometry.nb_poles_u(); m++) {
                refined->pole(m, k - degree_v-1) = refined->pole(m, k - degree_v);
                refined->weight(m, k - degree_v-1) = refined->weight(m, k - degree_v);
            }

            for (Index l = 1; l < degree_v + 1; l++) {
                const Index index = k - degree_v + l;
                auto alpha = refined->knot_v(-1+k + l) - knotsV[j];

                if (std::abs(alpha) < 1e-7) {
                    for (Index m = 0; m < geometry.nb_poles_u(); m++) {
                        refined->pole(m, index - 1) = refined->pole(m, index);
                        refined->weight(m, index - 1) = refined->weight(m, index);
                    }
                } else {
                    alpha = alpha / (refined->knot_v(k + l - 1) - geometry.knot_v(i + l - degree_v - 1));
                    for (Index m = 0; m < geometry.nb_poles_u(); m++) {
                        refined->pole(m, index - 1) = refined->pole(m, index - 1) * alpha + refined->pole(m, index) * (1 - alpha);
                        refined->weight(m, index - 1) = refined->weight(m, index - 1) * alpha + refined->weight(m, index) * (1 - alpha);
                    }
                }
            }

            refined->knot_v(-1 + k) = knotsV[j];

            k -= 1;
            j -= 1;
        }

        for (Index i = 0; i < refined->nb_poles(); i++) {
            refined->pole(i) = refined->pole(i) / refined->weight(i);
        }

        return refined;
    }

public:     // python
    static std::string python_name()
    {
        return "KnotRefinement" + std::to_string(TDimension) + "D";
    }

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = KnotRefinement<TDimension>;

        const std::string name = Type::python_name();

        py::class_<Type>(m, name.c_str())
            .def_static("insert_knots", &Type::insert_knots, "geometry"_a,
                "knots"_a)
            .def_static("insert_knots_u", &Type::insert_knots_u, "geometry"_a,
                "knots"_a)
            .def_static("insert_knots_v", &Type::insert_knots_v, "geometry"_a,
                "knots"_a)
        ;
    }
};

} // namespace anurbs
