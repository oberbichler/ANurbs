#pragma once

#include "../Define.h"

#include "KnotVector.h"
#include "Math.h"
#include "NurbsCurveShapeFunction.h"

#include <vector>

namespace ANurbs {

class NurbsSurfaceShapeFunction
{
private:        // variables
    int m_order;
    NurbsCurveShapeFunction m_shape_u;
    NurbsCurveShapeFunction m_shape_v;
    std::vector<double> m_weighted_sums;
    std::vector<double> m_values;
    int m_first_nonzero_pole_u;
    int m_first_nonzero_pole_v;

public:         // static methods
    static constexpr inline int nb_shapes(const int order) noexcept
    {
        return (1 + order) * (2 + order) / 2;
    }

    static constexpr inline int shape_index(const int derivative_u,
        const int derivative_v) noexcept
    {
        return derivative_v + (derivative_u + derivative_v) * (1 + derivative_u +
            derivative_v) / 2;
    }

private:        // methods
    double& weighted_sum(const int index)
    {
        return m_weighted_sums[index];
    }

    double& weighted_sum(const int derivative_u, const int derivative_v)
    {
        const int index = shape_index(derivative_u, derivative_v);

        return weighted_sum(index);
    }

    inline int index(const int derivative, const int poleU, const int poleV)
        const
    {
        using namespace Math;

        const int pole = Math::matrix_index(nb_nonzero_poles_u(),
            nb_nonzero_poles_v(), poleU, poleV);
        const int index = Math::matrix_index(nb_shapes(), nb_nonzero_poles(),
            derivative, pole);

        return index;
    }

    double& value(const int derivative, const int pole)
    {
        const int index = Math::matrix_index(nb_shapes(), nb_nonzero_poles(),
            derivative, pole);

        return m_values[index];
    }

    double& value(const int derivative, const int poleU, const int poleV)
    {
        const int index = this->index(derivative, poleU, poleV);

        return m_values[index];
    }

public:         // constructors
    NurbsSurfaceShapeFunction()
    {
    }

    NurbsSurfaceShapeFunction(const int degree_u, const int degree_v,
        const int order)
    {
        resize(degree_u, degree_v, order);
    }

public:         // methods
    void resize(const int degree_u, const int degree_v, const int order)
    {
        const int nb_shapes = this->nb_shapes(order);
        const int nb_nonzero_poles = (degree_u + 1) * (degree_v + 1);

        m_shape_u.resize(degree_u, order);
        m_shape_v.resize(degree_v, order);
        m_values.resize(nb_shapes * nb_nonzero_poles);
        m_weighted_sums.resize(nb_shapes);

        m_order = order;
    }

    int degree_u() const
    {
        return m_shape_u.degree();
    }

    int degree_v() const
    {
        return m_shape_v.degree();
    }

    int order() const
    {
        return m_order;
    }

    int nb_shapes() const
    {
        return nb_shapes(order());
    }

    int nb_nonzero_poles_u() const
    {
        return m_shape_u.nb_nonzero_poles();
    }

    int nb_nonzero_poles_v() const
    {
        return m_shape_v.nb_nonzero_poles();
    }

    int nb_nonzero_poles() const
    {
        return nb_nonzero_poles_u() * nb_nonzero_poles_v();
    }

    std::vector<std::pair<int, int>> nonzero_pole_indices() const
    {
        std::vector<std::pair<int, int>> indices(nb_nonzero_poles());

        for (int i = 0; i < nb_nonzero_poles_u(); i++) {
            for (int j = 0; j < nb_nonzero_poles_v(); j++) {
                int poleIndex = Math::matrix_index(nb_nonzero_poles_u(),
                    nb_nonzero_poles_v(), i, j);

                int poleU = first_nonzero_pole_u() + i;
                int poleV = first_nonzero_pole_v() + j;

                indices[poleIndex] = {poleU, poleV};
            }
        }

        return indices;
    }

    const double value(const int derivative, const int poleU, const int poleV)
        const
    {
        const int index = this->index(derivative, poleU, poleV);

        return m_values[index];
    }

    const double value(const int derivative, const int pole) const
    {
        const int index = Math::matrix_index(nb_shapes(), nb_nonzero_poles(),
            derivative, pole);

        return m_values[index];
    }

    double operator()(
        const int derivative,
        const int pole) const
    {
        return value(derivative, pole);
    }

    double operator()(
        const int derivative,
        const int poleU,
        const int poleV) const
    {
        return value(derivative, poleU, poleV);
    }

    int first_nonzero_pole_u() const
    {
        return m_first_nonzero_pole_u;
    }

    int last_nonzero_pole_u() const
    {
        return first_nonzero_pole_u() + degree_u();
    }

    int first_nonzero_pole_v() const
    {
        return m_first_nonzero_pole_v;
    }

    int last_nonzero_pole_v() const
    {
        return first_nonzero_pole_v() + degree_v();
    }

    void compute_at_span(
        const std::vector<double>& knots_u,
        const std::vector<double>& knots_v,
        const int span_u,
        const int span_v,
        const double u,
        const double v)
    {
        const int nbvalues = nb_shapes() * nb_nonzero_poles();

        std::fill(m_values.begin(), m_values.begin() + nbvalues, 0);

        m_first_nonzero_pole_u = span_u - degree_u() + 1;
        m_first_nonzero_pole_v = span_v - degree_v() + 1;

        // compute 1D shape functions

        m_shape_u.compute_at_span(knots_u, span_u, u);
        m_shape_v.compute_at_span(knots_v, span_v, v);

        // compute 2D shape functions

        for (int i = 0; i <= order(); i++) {
            for (int j = 0; j <= order() - i; j++) {
                for (int a = 0; a < nb_nonzero_poles_u(); a++) {
                    for (int b = 0; b < nb_nonzero_poles_v(); b++) {
                        const int index = shape_index(i, j);

                        value(index, a, b) = m_shape_u(i, a) * m_shape_v(j, b);
                    }
                }
            }
        }
    }

    void compute(const std::vector<double>& knots_u,
        const std::vector<double>& knots_v, const double u, const double v)
    {
        const int span_u = KnotVector::lower_span(degree_u(), knots_u, u);
        const int span_v = KnotVector::lower_span(degree_v(), knots_v, v);

        compute_at_span(knots_u, knots_v, span_u, span_v, u, v);
    }

    template <typename TWeights>
    void compute_at_span(const std::vector<double>& knots_u,
        const std::vector<double>& knots_v, const int span_u, const int span_v,
        const TWeights& weights, const double u, const double v)
    {
        using Math::binom;

        // compute B-Spline shape

        compute_at_span(knots_u, knots_v, span_u, span_v, u, v);

        // apply weights

        for (int shape = 0; shape < nb_shapes(); shape++) {
            weighted_sum(shape) = double(0);

            for (int i = 0; i < nb_nonzero_poles_u(); i++) {
                for (int j = 0; j < nb_nonzero_poles_v(); j++) {
                    const int poleU = first_nonzero_pole_u() + i;
                    const int poleV = first_nonzero_pole_v() + j;

                    const double weight = weights(poleU, poleV);
                    value(shape, i, j) *= weight;
                    weighted_sum(shape) += value(shape, i, j);
                }
            }
        }

        for (int k = 0; k <= order(); k++) {
            for (int l = 0; l <= order() - k; l++) {
                const int shape = shape_index(k, l);

                for (int j = 1; j <= l; j++) {
                    const int index = shape_index(k, l - j);

                    double a = binom(l, j) * weighted_sum(0, j);

                    for (int p = 0; p < nb_nonzero_poles(); p++) {
                        value(shape, p) -= a * value(index, p);
                    }
                }

                for (int i = 1; i <= k; i++) {
                    const int index = shape_index(k - i, l);

                    double a = binom(k, i) * weighted_sum(i, 0);

                    for (int p = 0; p < nb_nonzero_poles(); p++) {
                        value(shape, p) -= a * value(index, p);
                    }
                }

                for (int i = 1; i <= k; i++) {
                    const double a = binom(k, i);

                    for (int j = 1; j <= l; j++) {
                        const int index = shape_index(k - i, l - j);

                        const double b = a * binom(l, j) *
                            weighted_sum(i, j);

                        for (int p = 0; p < nb_nonzero_poles(); p++) {
                            value(shape, p) -= b * value(index, p);
                        }
                    }
                }

                for (int p = 0; p < nb_nonzero_poles(); p++) {
                    value(shape, p) /= weighted_sum(0);
                }
            }
        }
    }

    template <typename TWeights>
    void compute(const std::vector<double>& knots_u,
        const std::vector<double>& knots_v, const TWeights& weights,
        const double u, const double v)
    {
        const int span_u = KnotVector::lower_span(degree_u(), knots_u, u);
        const int span_v = KnotVector::lower_span(degree_v(), knots_v, v);

        compute_at_span(knots_u, knots_v, span_u, span_v, weights, u, v);
    }

    static std::pair<std::vector<std::pair<int, int>>, MatrixXd> get(
        const int degree_u, const int degree_v, const int order,
        const std::vector<double>& knots_u, const std::vector<double>& knots_v,
        const double u, const double v)
    {
        NurbsSurfaceShapeFunction shape_function(degree_u, degree_v, order);

        shape_function.compute(knots_u, knots_v, u, v);
        
        const auto nonzero_pole_indices = shape_function.nonzero_pole_indices();
        
        const Map<MatrixXd> values(shape_function.m_values.data(),
            shape_function.nb_shapes(), shape_function.nb_nonzero_poles());
        
        return {nonzero_pole_indices, values};
    }

    template <typename TWeights>
    static std::pair<std::vector<std::pair<int, int>>, MatrixXd> get(
        const int degree_u, const int degree_v, const int order,
        const std::vector<double>& knots_u, const std::vector<double>& knots_v,
        const TWeights& weights, const double u, const double v)
    {
        NurbsSurfaceShapeFunction shape_function(degree_u, degree_v, order);

        shape_function.compute<TWeights>(knots_u, knots_v, weights, u, v);
        
        const auto nonzero_pole_indices = shape_function.nonzero_pole_indices();
        
        const Map<MatrixXd> values(shape_function.m_values.data(),
            shape_function.nb_shapes(), shape_function.nb_nonzero_poles());
        
        return {nonzero_pole_indices, values};
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = NurbsSurfaceShapeFunction;
        
        m.def("shape_functions", [](const int degree_u, const int degree_v,
            const int order, const std::vector<double>& knots_u,
            const std::vector<double>& knots_v, const double u, const double v)
            { return Type::get(degree_u, degree_v, order, knots_u, knots_v,
            u, v); }, "degree_u"_a, "degree_v"_a, "order"_a, "knots_u"_a,
            "knots_v"_a, "u"_a, "v"_a);
        m.def("shape_functions", &Type::get<MatrixXd>, "degree_u"_a,
            "degree_v"_a, "order"_a, "knots_u"_a, "knots_v"_a, "weights"_a,
            "u"_a, "v"_a);
    }
};

} // namespace ANurbs
