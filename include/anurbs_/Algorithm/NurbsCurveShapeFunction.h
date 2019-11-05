#pragma once

#include "../Define.h"

#include "Nurbs.h"
#include "Math.h"

#include <vector>

namespace anurbs {

class NurbsCurveShapeFunction
{
private:    // variables
    Index m_degree;
    Index m_order;
    std::vector<double> m_values;
    std::vector<double> m_left;
    std::vector<double> m_right;
    std::vector<double> m_ndu;
    std::vector<double> m_a;
    std::vector<double> m_b;
    Index m_first_nonzero_pole;

private:    // methods
    double& values(const Index order, const Index pole)
    {
        const Index index = Math::single_index(nb_shapes(), nb_nonzero_poles(),
            order, pole);

        return m_values[index];
    }

    double& ndu(const Index i, const Index j)
    {
        const Index index = Math::single_index(nb_shapes(), nb_nonzero_poles(), i, j);

        return m_ndu[index];
    }

    void clear_values()
    {
        const Index nb_values = nb_nonzero_poles() * nb_shapes();

        std::fill(m_values.begin(), m_values.begin() + nb_values, 0);
    }

public:     // constructors
    NurbsCurveShapeFunction()
    {
    }

    NurbsCurveShapeFunction(const Index degree, const Index order)
    {
        resize(degree, order);
    }

public:     // methods
    void resize(const Index degree, const Index order)
    {
        m_values.resize((order + 1) * (degree + 1));
        m_left.resize(degree);
        m_right.resize(degree);
        m_ndu.resize((degree + 1) * (degree + 1));
        m_a.resize(degree + 1);
        m_b.resize(degree + 1);

        m_degree = degree;
        m_order = order;
    }

    Index degree() const
    {
        return m_degree;
    }

    Index order() const
    {
        return m_order;
    }

    Index nb_nonzero_poles() const
    {
        return degree() + 1;
    }

    Index nb_shapes() const
    {
        return order() + 1;
    }

    double operator()(const Index order, const Index pole) const
    {
        return value(order, pole);
    }

    double& value(const Index order, const Index pole)
    {
        const Index index = Math::single_index(nb_shapes(), nb_nonzero_poles(),
            order, pole);

        return m_values[index];
    }

    double value(const Index order, const Index pole) const
    {
        const Index index = Math::single_index(nb_shapes(), nb_nonzero_poles(),
            order, pole);

        return m_values[index];
    }

    Index first_nonzero_pole() const
    {
        return m_first_nonzero_pole;
    }

    Index last_nonzero_pole() const
    {
        return first_nonzero_pole() + degree();
    }

    std::vector<Index> nonzero_pole_indices() const
    {
        std::vector<Index> indices(nb_nonzero_poles());

        for (Index i = 0; i < nb_nonzero_poles(); i++) {
            indices[i] = first_nonzero_pole() + i;
        }

        return indices;
    }

    template <typename Knots>
    void compute_at_span(const Knots& knots, const Index span, const double t)
    {
        clear_values();

        m_first_nonzero_pole = span - degree() + 1;

        // compute B-Spline shape

        ndu(0, 0) = 1.0;

        for (Index j = 0; j < degree(); j++) {
            m_left[j] = t - knots[span - j];
            m_right[j] = knots[span + j + 1] - t;

            double saved = 0.0;

            for (Index r = 0; r <= j; r++) {
                ndu(j + 1, r) = m_right[r] + m_left[j - r];

                double temp = ndu(r, j) / ndu(j + 1, r);

                ndu(r, j + 1) = saved + m_right[r] * temp;

                saved = m_left[j - r] * temp;
            }

            ndu(j + 1, j + 1) = saved;
        }

        for (Index j = 0; j < nb_nonzero_poles(); j++) {
            values(0, j) = ndu(j, degree());
        }

        auto& a = m_a;
        auto& b = m_b;

        for (Index r = 0; r < nb_nonzero_poles(); r++) {
            a[0] = 1.0;

            for (Index k = 1; k < nb_shapes(); k++) {
                double& value = values(k, r);

                Index rk = r - k;
                Index pk = degree() - k;

                if (r >= k) {
                    b[0] = a[0] / ndu(pk + 1, rk);
                    value = b[0] * ndu(rk, pk);
                }

                Index j1 = r >= k - 1 ? 1 : k - r;
                Index j2 = r <= pk + 1 ? k : nb_nonzero_poles() - r;

                for (Index j = j1; j < j2; j++) {
                    b[j] = (a[j] - a[j - 1]) / ndu(pk + 1, rk + j);
                    value += b[j] * ndu(rk + j, pk);
                }

                if (r <= pk) {
                    b[k] = -a[k - 1] / ndu(pk + 1, r);
                    value += b[k] * ndu(r, pk);
                }

                std::swap(a, b);
            }
        }

        Index s = degree();

        for (Index k = 1; k < nb_shapes(); k++) {
            for (Index j = 0; j < nb_nonzero_poles(); j++) {
                values(k, j) *= s;
            }
            s *= degree() - k;
        }
    }

    template <typename TKnots, typename TWeights>
    void compute_at_span(const TKnots& knots, const Index span,
        const TWeights& weights, const double t)
    {
        using Math::binom;

        // compute B-Spline shape

        compute_at_span(knots, span, t);

        // compute weighted sum

        std::vector<double> weighted_sums(nb_shapes());

        for (Index k = 0; k < nb_shapes(); k++) {
            weighted_sums[k] = 0;

            for (Index i = 0; i < nb_nonzero_poles(); i++) {
                value(k, i) *= weights(m_first_nonzero_pole + i);
                weighted_sums[k] += value(k, i);
            }
        }

        for (Index k = 0; k < nb_shapes(); k++) {
            for (Index i = 1; i <= k; i++) {
                const double a = binom(k, i) * weighted_sums[i];

                for (Index p = 0; p < nb_nonzero_poles(); p++) {
                    value(k, p) -= a * value(k - i, p);
                }
            }

            for (Index p = 0; p < nb_nonzero_poles(); p++) {
                value(k, p) /= weighted_sums[0];
            }
        }
    }

    void compute(const std::vector<double>& knots, const double t)
    {
        const Index span = Nurbs::upper_span(degree(), knots, t);

        compute_at_span(knots, span, t);
    }

    template <typename TWeights>
    void compute(const std::vector<double>& knots, const TWeights& weights,
        const double t)
    {
        const Index span = Nurbs::upper_span(degree(), knots, t);

        compute_at_span(knots, span, weights, t);
    }

    static std::pair<std::vector<Index>, linear_algebra::MatrixXd> get(const Index degree,
        const Index order, const std::vector<double>& knots, const double t)
    {
        NurbsCurveShapeFunction shape_function(degree, order);

        shape_function.compute(knots, t);

        const auto nonzero_pole_indices = shape_function.nonzero_pole_indices();

        const linear_algebra::Map<linear_algebra::MatrixXd> values(shape_function.m_values.data(),
            shape_function.nb_shapes(), shape_function.nb_nonzero_poles());

        return {nonzero_pole_indices, values};
    }

    template <typename TWeights>
    static std::pair<std::vector<Index>, linear_algebra::MatrixXd> get(const Index degree,
        const Index order, const std::vector<double>& knots,
        const TWeights& weights, const double t)
    {
        NurbsCurveShapeFunction shape_function(degree, order);

        shape_function.compute(knots, weights, t);
        
        const auto nonzero_pole_indices = shape_function.nonzero_pole_indices();
        
        const linear_algebra::Map<linear_algebra::MatrixXd> values(shape_function.m_values.data(),
            shape_function.nb_shapes(), shape_function.nb_nonzero_poles());
        
        return {nonzero_pole_indices, values};
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = NurbsCurveShapeFunction;
        
        m.def("shape_functions", [](const Index degree, const Index order,
            const std::vector<double>& knots, const double t) {
            return Type::get(degree, order, knots, t); }, "degree"_a, "order"_a,
            "knots"_a, "t"_a);
        m.def("shape_functions", &Type::get<linear_algebra::VectorXd>, "degree"_a, "order"_a,
            "knots"_a, "weights"_a, "t"_a);
    }
};

} // namespace anurbs