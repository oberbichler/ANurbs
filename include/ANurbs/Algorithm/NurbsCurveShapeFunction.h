#pragma once

#include "../Define.h"

#include "Nurbs.h"
#include "Math.h"

#include <vector>

namespace ANurbs {

class NurbsCurveShapeFunction
{
private:    // variables
    int m_degree;
    int m_order;
    std::vector<double> m_values;
    std::vector<double> m_left;
    std::vector<double> m_right;
    std::vector<double> m_ndu;
    std::vector<double> m_a;
    std::vector<double> m_b;
    int m_first_nonzero_pole;

private:    // methods
    double& values(const int order, const int pole)
    {
        const int index = Math::single_index(nb_shapes(), nb_nonzero_poles(),
            order, pole);

        return m_values[index];
    }

    double& ndu(const int i, const int j)
    {
        const int index = Math::single_index(nb_shapes(), nb_nonzero_poles(), i, j);

        return m_ndu[index];
    }

    void clear_values()
    {
        const int nb_values = nb_nonzero_poles() * nb_shapes();

        std::fill(m_values.begin(), m_values.begin() + nb_values, 0);
    }

public:     // constructors
    NurbsCurveShapeFunction()
    {
    }

    NurbsCurveShapeFunction(const int degree, const int order)
    {
        resize(degree, order);
    }

public:     // methods
    void resize(const int degree, const int order)
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

    int degree() const
    {
        return m_degree;
    }

    int order() const
    {
        return m_order;
    }

    int nb_nonzero_poles() const
    {
        return degree() + 1;
    }

    int nb_shapes() const
    {
        return order() + 1;
    }

    double operator()(const int order, const int pole) const
    {
        return value(order, pole);
    }

    double value(const int order, const int pole) const
    {
        int index = Math::single_index(nb_shapes(), nb_nonzero_poles(),
            order, pole);

        return m_values[index];
    }

    int first_nonzero_pole() const
    {
        return m_first_nonzero_pole;
    }

    int last_nonzero_pole() const
    {
        return first_nonzero_pole() + degree();
    }

    std::vector<int> nonzero_pole_indices() const
    {
        std::vector<int> indices(nb_nonzero_poles());

        for (int i = 0; i < nb_nonzero_poles(); i++) {
            indices[i] = first_nonzero_pole() + i;
        }

        return indices;
    }

    template <typename Knots>
    void compute_at_span(const Knots& knots, const int span, const double t)
    {
        clear_values();

        m_first_nonzero_pole = span - degree() + 1;

        // compute B-Spline shape

        ndu(0, 0) = 1.0;

        for (int j = 0; j < degree(); j++) {
            m_left[j] = t - knots[span - j];
            m_right[j] = knots[span + j + 1] - t;

            double saved = 0.0;

            for (int r = 0; r <= j; r++) {
                ndu(j + 1, r) = m_right[r] + m_left[j - r];

                double temp = ndu(r, j) / ndu(j + 1, r);

                ndu(r, j + 1) = saved + m_right[r] * temp;

                saved = m_left[j - r] * temp;
            }

            ndu(j + 1, j + 1) = saved;
        }

        for (int j = 0; j < nb_nonzero_poles(); j++) {
            values(0, j) = ndu(j, degree());
        }

        auto& a = m_a;
        auto& b = m_b;

        for (int r = 0; r < nb_nonzero_poles(); r++) {
            a[0] = 1.0;

            for (int k = 1; k < nb_shapes(); k++) {
                double& value = values(k, r);

                int rk = r - k;
                int pk = degree() - k;

                if (r >= k) {
                    b[0] = a[0] / ndu(pk + 1, rk);
                    value = b[0] * ndu(rk, pk);
                }

                int j1 = r >= k - 1 ? 1 : k - r;
                int j2 = r <= pk + 1 ? k : nb_nonzero_poles() - r;

                for (int j = j1; j < j2; j++) {
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

        int s = degree();

        for (int k = 1; k < nb_shapes(); k++) {
            for (int j = 0; j < nb_nonzero_poles(); j++) {
                values(k, j) *= s;
            }
            s *= degree() - k;
        }
    }

    template <typename TKnots, typename TWeights>
    void compute_at_span(const TKnots& knots, const int span,
        const TWeights& weights, const double t)
    {
        // compute B-Spline shape

        compute_at_span(knots, span, t);

        // compute weighted sum

        double weightedSum {0};

        for (int i = 0; i < nb_nonzero_poles(); i++) {
            m_values[i] *= weights(i);
            weightedSum += m_values[i];
        }

        // apply weights

        for (int i = 0; i < nb_nonzero_poles(); i++) {
            m_values[i] /= weightedSum;
        }
    }

    void compute(const std::vector<double>& knots, const double t)
    {
        const int span = Nurbs::upper_span(degree(), knots, t);

        compute_at_span(knots, span, t);
    }

    template <typename TWeights>
    void compute(const std::vector<double>& knots, const TWeights& weights,
        const double t)
    {
        const int span = Nurbs::upper_span(degree(), knots, t);

        compute_at_span(knots, span, weights, t);
    }

    static std::pair<std::vector<int>, MatrixXd> get(const int degree,
        const int order, const std::vector<double>& knots, const double t)
    {
        NurbsCurveShapeFunction shape_function(degree, order);

        shape_function.compute(knots, t);

        const auto nonzero_pole_indices = shape_function.nonzero_pole_indices();

        const Map<MatrixXd> values(shape_function.m_values.data(),
            shape_function.nb_shapes(), shape_function.nb_nonzero_poles());

        return {nonzero_pole_indices, values};
    }

    template <typename TWeights>
    static std::pair<std::vector<int>, MatrixXd> get(const int degree,
        const int order, const std::vector<double>& knots,
        const TWeights& weights, const double t)
    {
        NurbsCurveShapeFunction shape_function(degree, order);

        shape_function.compute(knots, weights, t);
        
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

        using Type = NurbsCurveShapeFunction;
        
        m.def("shape_functions", [](const int degree, const int order,
            const std::vector<double>& knots, const double t) {
            return Type::get(degree, order, knots, t); }, "degree"_a, "order"_a,
            "knots"_a, "t"_a);
        m.def("shape_functions", &Type::get<VectorXd>, "degree"_a, "order"_a,
            "knots"_a, "weights"_a, "t"_a);
    }
};

} // namespace ANurbs