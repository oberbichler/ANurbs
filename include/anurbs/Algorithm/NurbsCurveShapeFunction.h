#pragma once

#include "../Define.h"

#include "Math.h"
#include "Nurbs.h"

#include <vector>

namespace anurbs {

class NurbsCurveShapeFunction {
private: // variables
    Index m_degree;
    Index m_order;
    Eigen::MatrixXd m_values;
    Eigen::VectorXd m_left;
    Eigen::VectorXd m_right;
    Eigen::MatrixXd m_ndu;
    Eigen::VectorXd m_a;
    Eigen::VectorXd m_b;
    Index m_first_nonzero_pole;

private: // methods
    double& ndu(const Index i, const Index j)
    {
        return m_ndu(i, j);
    }

    void clear_values()
    {
        m_values.setZero();
    }

public: // constructors
    NurbsCurveShapeFunction()
    {
    }

    NurbsCurveShapeFunction(const Index degree, const Index order)
    {
        resize(degree, order);
    }

public: // methods
    void resize(const Index degree, const Index order)
    {
        m_values.resize((order + 1), (degree + 1));
        m_left.resize(degree);
        m_right.resize(degree);
        m_ndu.resize((degree + 1), (degree + 1));
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

    double& value(const Index order, const Index pole)
    {
        return m_values(order, pole);
    }

    double value(const Index order, const Index pole) const
    {
        return m_values(order, pole);
    }

    Eigen::Ref<const Eigen::MatrixXd> values() const
    {
        return m_values;
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

    void compute_at_span(Eigen::Ref<const Eigen::VectorXd> knots, const Index span, const double t)
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
            value(0, j) = ndu(j, degree());
        }

        auto& a = m_a;
        auto& b = m_b;

        for (Index r = 0; r < nb_nonzero_poles(); r++) {
            a[0] = 1.0;

            for (Index k = 1; k < nb_shapes(); k++) {
                double& v = value(k, r);

                Index rk = r - k;
                Index pk = degree() - k;

                if (r >= k) {
                    b[0] = a[0] / ndu(pk + 1, rk);
                    v = b[0] * ndu(rk, pk);
                }

                Index j1 = r >= k - 1 ? 1 : k - r;
                Index j2 = r <= pk + 1 ? k : nb_nonzero_poles() - r;

                for (Index j = j1; j < j2; j++) {
                    b[j] = (a[j] - a[j - 1]) / ndu(pk + 1, rk + j);
                    v += b[j] * ndu(rk + j, pk);
                }

                if (r <= pk) {
                    b[k] = -a[k - 1] / ndu(pk + 1, r);
                    v += b[k] * ndu(r, pk);
                }

                std::swap(a, b);
            }
        }

        Index s = degree();

        for (Index k = 1; k < nb_shapes(); k++) {
            for (Index j = 0; j < nb_nonzero_poles(); j++) {
                value(k, j) *= s;
            }
            s *= degree() - k;
        }
    }

    void compute_at_span(Eigen::Ref<const Eigen::VectorXd> knots, const Index span, Eigen::Ref<const Eigen::VectorXd> weights, const double t)
    {
        using Math::binom;

        // compute B-Spline shape

        compute_at_span(knots, span, t);

        // compute weighted sum

        Eigen::VectorXd weighted_sums(nb_shapes());

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

    void compute(Eigen::Ref<const Eigen::VectorXd> knots, const double t)
    {
        const Index span = Nurbs::upper_span(degree(), knots, t);

        compute_at_span(knots, span, t);
    }

    void compute(Eigen::Ref<const Eigen::VectorXd> knots, Eigen::Ref<const Eigen::VectorXd> weights, const double t)
    {
        const Index span = Nurbs::upper_span(degree(), knots, t);

        compute_at_span(knots, span, weights, t);
    }

    static std::pair<std::vector<Index>, Eigen::MatrixXd> get(const Index degree, const Index order, Eigen::Ref<const Eigen::VectorXd> knots, const double t)
    {
        NurbsCurveShapeFunction shape_function(degree, order);

        shape_function.compute(knots, t);

        const auto nonzero_pole_indices = shape_function.nonzero_pole_indices();

        return {nonzero_pole_indices, shape_function.m_values};
    }

    static std::pair<std::vector<Index>, Eigen::MatrixXd> get_weighted(const Index degree, const Index order, Eigen::Ref<const Eigen::VectorXd> knots, const Eigen::Ref<const Eigen::VectorXd> weights, const double t)
    {
        NurbsCurveShapeFunction shape_function(degree, order);

        shape_function.compute(knots, weights, t);

        const auto nonzero_pole_indices = shape_function.nonzero_pole_indices();

        return {nonzero_pole_indices, shape_function.m_values};
    }

public: // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = NurbsCurveShapeFunction;

        m.def("shape_functions", &Type::get, "degree"_a, "order"_a, "knots"_a, "t"_a);
        m.def("shape_functions", &Type::get_weighted, "degree"_a, "order"_a, "knots"_a, "weights"_a, "t"_a);
    }
};

} // namespace anurbs