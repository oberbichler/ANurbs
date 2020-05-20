#pragma once

#include "../Define.h"

#include "Math.h"
#include "Nurbs.h"
#include "NurbsCurveShapeFunction.h"

#include <vector>

namespace anurbs {

class NurbsSurfaceShapeFunction {
private: // variables
    Index m_order;
    NurbsCurveShapeFunction m_shape_u;
    NurbsCurveShapeFunction m_shape_v;
    Eigen::VectorXd m_weighted_sums;
    Eigen::MatrixXd m_values;
    Index m_first_nonzero_pole_u;
    Index m_first_nonzero_pole_v;

public: // static methods
    static constexpr inline Index nb_shapes(const Index order) noexcept
    {
        return (1 + order) * (2 + order) / 2;
    }

    static constexpr inline Index shape_index(const Index derivative_u, const Index derivative_v) noexcept
    {
        return derivative_v + (derivative_u + derivative_v) * (1 + derivative_u + derivative_v) / 2;
    }

private: // methods
    double& weighted_sum(const Index index)
    {
        return m_weighted_sums(index);
    }

    double& weighted_sum(const Index derivative_u, const Index derivative_v)
    {
        const Index index = shape_index(derivative_u, derivative_v);

        return weighted_sum(index);
    }

    inline Index index(const Index derivative, const Index poleU, const Index poleV) const
    {
        using namespace Math;

        const Index pole = Math::single_index(nb_nonzero_poles_u(), nb_nonzero_poles_v(), poleU, poleV);
        const Index index = Math::single_index(nb_shapes(), nb_nonzero_poles(), derivative, pole);

        return index;
    }

public: // constructors
    NurbsSurfaceShapeFunction()
    {
    }

    NurbsSurfaceShapeFunction(const Index degree_u, const Index degree_v, const Index order)
    {
        resize(degree_u, degree_v, order);
    }

public: // methods
    void resize(const Index degree_u, const Index degree_v, const Index order)
    {
        const Index nb_shapes = this->nb_shapes(order);
        const Index nb_nonzero_poles = (degree_u + 1) * (degree_v + 1);

        m_shape_u.resize(degree_u, order);
        m_shape_v.resize(degree_v, order);
        m_values.resize(nb_shapes, nb_nonzero_poles);
        m_weighted_sums.resize(nb_shapes);

        m_order = order;
    }

    Index degree_u() const
    {
        return m_shape_u.degree();
    }

    Index degree_v() const
    {
        return m_shape_v.degree();
    }

    Index order() const
    {
        return m_order;
    }

    Index nb_shapes() const
    {
        return nb_shapes(order());
    }

    Index nb_nonzero_poles_u() const
    {
        return m_shape_u.nb_nonzero_poles();
    }

    Index nb_nonzero_poles_v() const
    {
        return m_shape_v.nb_nonzero_poles();
    }

    Index nb_nonzero_poles() const
    {
        return nb_nonzero_poles_u() * nb_nonzero_poles_v();
    }

    std::vector<std::pair<Index, Index>> nonzero_pole_indices() const
    {
        std::vector<std::pair<Index, Index>> indices(nb_nonzero_poles());

        for (Index i = 0; i < nb_nonzero_poles_u(); i++) {
            for (Index j = 0; j < nb_nonzero_poles_v(); j++) {
                Index poleIndex = Math::single_index(nb_nonzero_poles_u(), nb_nonzero_poles_v(), i, j);

                Index poleU = first_nonzero_pole_u() + i;
                Index poleV = first_nonzero_pole_v() + j;

                indices[poleIndex] = {poleU, poleV};
            }
        }

        return indices;
    }

    Eigen::Ref<const Eigen::MatrixXd> values() const
    {
        return m_values;
    }

    double value(const Index derivative, const Index pole) const
    {
        return m_values(derivative, pole);
    }

    double& value(const Index derivative, const Index pole)
    {
        return m_values(derivative, pole);
    }

    double value(const Index derivative, const Index poleU, const Index poleV) const
    {
        const Index pole = Math::single_index(nb_nonzero_poles_u(), nb_nonzero_poles_v(), poleU, poleV);

        return m_values(derivative, pole);
    }

    double& value(const Index derivative, const Index poleU, const Index poleV)
    {
        const Index pole = Math::single_index(nb_nonzero_poles_u(), nb_nonzero_poles_v(), poleU, poleV);

        return m_values(derivative, pole);
    }

    Index first_nonzero_pole_u() const
    {
        return m_first_nonzero_pole_u;
    }

    Index last_nonzero_pole_u() const
    {
        return first_nonzero_pole_u() + degree_u();
    }

    Index first_nonzero_pole_v() const
    {
        return m_first_nonzero_pole_v;
    }

    Index last_nonzero_pole_v() const
    {
        return first_nonzero_pole_v() + degree_v();
    }

    void compute_at_span(
        Eigen::Ref<const Eigen::VectorXd> knots_u,
        Eigen::Ref<const Eigen::VectorXd> knots_v,
        const Index span_u,
        const Index span_v,
        const double u,
        const double v)
    {
        m_values.setZero();

        m_first_nonzero_pole_u = span_u - degree_u() + 1;
        m_first_nonzero_pole_v = span_v - degree_v() + 1;

        // compute 1D shape functions

        m_shape_u.compute_at_span(knots_u, span_u, u);
        m_shape_v.compute_at_span(knots_v, span_v, v);

        // compute 2D shape functions

        for (Index i = 0; i <= order(); i++) {
            for (Index j = 0; j <= order() - i; j++) {
                for (Index a = 0; a < nb_nonzero_poles_u(); a++) {
                    for (Index b = 0; b < nb_nonzero_poles_v(); b++) {
                        const Index index = shape_index(i, j);

                        value(index, a, b) = m_shape_u.value(i, a) * m_shape_v.value(j, b);
                    }
                }
            }
        }
    }

    void compute(Eigen::Ref<const Eigen::VectorXd> knots_u, Eigen::Ref<const Eigen::VectorXd> knots_v, const double u, const double v)
    {
        const Index span_u = Nurbs::lower_span(degree_u(), knots_u, u);
        const Index span_v = Nurbs::lower_span(degree_v(), knots_v, v);

        compute_at_span(knots_u, knots_v, span_u, span_v, u, v);
    }

    void compute_at_span(
        Eigen::Ref<const Eigen::VectorXd> knots_u,
        Eigen::Ref<const Eigen::VectorXd> knots_v,
        const Index span_u,
        const Index span_v,
        Eigen::Ref<const Eigen::MatrixXd> weights,
        const double u,
        const double v)
    {
        using Math::binom;

        // compute B-Spline shape

        compute_at_span(knots_u, knots_v, span_u, span_v, u, v);

        // apply weights

        for (Index shape = 0; shape < nb_shapes(); shape++) {
            weighted_sum(shape) = double(0);

            for (Index i = 0; i < nb_nonzero_poles_u(); i++) {
                for (Index j = 0; j < nb_nonzero_poles_v(); j++) {
                    const Index poleU = first_nonzero_pole_u() + i;
                    const Index poleV = first_nonzero_pole_v() + j;

                    value(shape, i, j) *= weights(poleU, poleV);
                    weighted_sum(shape) += value(shape, i, j);
                }
            }
        }

        for (Index k = 0; k <= order(); k++) {
            for (Index l = 0; l <= order() - k; l++) {
                const Index shape = shape_index(k, l);

                for (Index j = 1; j <= l; j++) {
                    const Index index = shape_index(k, l - j);

                    const double a = binom(l, j) * weighted_sum(0, j);

                    for (Index p = 0; p < nb_nonzero_poles(); p++) {
                        value(shape, p) -= a * value(index, p);
                    }
                }

                for (Index i = 1; i <= k; i++) {
                    const Index index = shape_index(k - i, l);

                    const double a = binom(k, i) * weighted_sum(i, 0);

                    for (Index p = 0; p < nb_nonzero_poles(); p++) {
                        value(shape, p) -= a * value(index, p);
                    }
                }

                for (Index i = 1; i <= k; i++) {
                    const double a = binom(k, i);

                    for (Index j = 1; j <= l; j++) {
                        const Index index = shape_index(k - i, l - j);

                        const double b = a * binom(l, j) * weighted_sum(i, j);

                        for (Index p = 0; p < nb_nonzero_poles(); p++) {
                            value(shape, p) -= b * value(index, p);
                        }
                    }
                }

                for (Index p = 0; p < nb_nonzero_poles(); p++) {
                    value(shape, p) /= weighted_sum(0);
                }
            }
        }
    }

    void compute(Eigen::Ref<const Eigen::VectorXd> knots_u, Eigen::Ref<const Eigen::VectorXd> knots_v, Eigen::Ref<const Eigen::MatrixXd> weights, const double u, const double v)
    {
        const Index span_u = Nurbs::lower_span(degree_u(), knots_u, u);
        const Index span_v = Nurbs::lower_span(degree_v(), knots_v, v);

        compute_at_span(knots_u, knots_v, span_u, span_v, weights, u, v);
    }

    static std::pair<std::vector<std::pair<Index, Index>>, Eigen::MatrixXd> get(const Index degree_u, const Index degree_v, const Index order, Eigen::Ref<const Eigen::VectorXd> knots_u, Eigen::Ref<const Eigen::VectorXd> knots_v, const double u, const double v)
    {
        NurbsSurfaceShapeFunction shape_function(degree_u, degree_v, order);

        shape_function.compute(knots_u, knots_v, u, v);

        const auto nonzero_pole_indices = shape_function.nonzero_pole_indices();

        return {nonzero_pole_indices, shape_function.m_values};
    }

    static std::pair<std::vector<std::pair<Index, Index>>, Eigen::MatrixXd> get_weighted(const Index degree_u, const Index degree_v, const Index order, Eigen::Ref<const Eigen::VectorXd> knots_u, Eigen::Ref<const Eigen::VectorXd> knots_v, Eigen::Ref<const Eigen::MatrixXd> weights, const double u, const double v)
    {
        NurbsSurfaceShapeFunction shape_function(degree_u, degree_v, order);

        shape_function.compute(knots_u, knots_v, weights, u, v);

        const auto nonzero_pole_indices = shape_function.nonzero_pole_indices();

        return {nonzero_pole_indices, shape_function.m_values};
    }

public: // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = NurbsSurfaceShapeFunction;

        m.def("shape_functions", &Type::get, "degree_u"_a, "degree_v"_a, "order"_a, "knots_u"_a, "knots_v"_a, "u"_a, "v"_a);
        m.def("shape_functions", &Type::get_weighted, "degree_u"_a, "degree_v"_a, "order"_a, "knots_u"_a, "knots_v"_a, "weights"_a, "u"_a, "v"_a);
    }
};

} // namespace anurbs
