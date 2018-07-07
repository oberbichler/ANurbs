#pragma once

#include "Knots.h"
#include "Math.h"
#include "Util.h"
#include "VectorMath.h"

#include <functional>
#include <vector>

namespace ANurbs {

template <typename TScalar>
class CurveShapeEvaluator
{
public:
    using ScalarType = TScalar;

private:
    std::size_t m_degree;
    std::size_t m_order;
    std::vector<ScalarType> m_values;
    std::vector<ScalarType> m_left;
    std::vector<ScalarType> m_right;
    std::vector<ScalarType> m_ndu;
    std::vector<ScalarType> m_a;
    std::vector<ScalarType> m_b;
    std::size_t m_firstNonzeroPole;

private:
    ScalarType&
    Values(
        const std::size_t& order,
        const std::size_t& pole
    )
    {
        std::size_t index = Math::MatrixIndex(NbShapes(), NbNonzeroPoles(),
            order, pole);

        return m_values[index];
    }

    ScalarType&
    Ndu(
        const std::size_t& i,
        const std::size_t& j
    )
    {
        std::size_t index = Math::MatrixIndex(NbShapes(), NbNonzeroPoles(),
            i, j);

        return m_ndu[index];
    }

    void
    ClearValues(
    )
    {
        std::size_t nbValues = NbNonzeroPoles() * NbShapes();

        std::fill(m_values.begin(), m_values.begin() + nbValues, 0);
    }

public:
    CurveShapeEvaluator(
    )
    {
    }

    CurveShapeEvaluator(
        const std::size_t& degree,
        const std::size_t& order
    )
    {
        Resize(degree, order);
    }

    void
    Resize(
        const std::size_t& degree,
        const std::size_t& order
    )
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

    std::size_t
    Degree(
    ) const
    {
        return m_degree;
    }

    std::size_t
    Order(
    ) const
    {
        return m_order;
    }

    std::size_t
    NbNonzeroPoles(
    ) const
    {
        return Degree() + 1;
    }

    std::size_t
    NbShapes(
    ) const
    {
        return Order() + 1;
    }

    ScalarType
    operator()(
        const std::size_t& order,
        const std::size_t& pole
    ) const
    {
        return Value(order, pole);
    }

    ScalarType
    Value(
        const std::size_t& order,
        const std::size_t& pole
    ) const
    {
        std::size_t index = Math::MatrixIndex(NbShapes(), NbNonzeroPoles(),
            order, pole);

        return m_values[index];
    }

    std::size_t
    FirstNonzeroPole(
    ) const
    {
        return m_firstNonzeroPole;
    }

    std::size_t
    LastNonzeroPole(
    ) const
    {
        return FirstNonzeroPole() + Degree();
    }
    
    std::vector<std::size_t>
    NonZeroPoleIndices(
    ) const
    {
        std::vector<std::size_t> indices(NbNonzeroPoles());

        for (std::size_t i = 0; i < NbNonzeroPoles(); i++) {
            indices[i] = FirstNonzeroPole() + i;
        }

        return indices;
    }

    template <typename Knots>
    void
    ComputeAtSpan(
        const Knots& knots,
        const std::size_t& span,
        const ScalarType& t
    )
    {
        ClearValues();

        m_firstNonzeroPole = span - Degree() + 1;

        // compute B-Spline shape

        Ndu(0, 0) = 1.0;

        for (std::size_t j = 0; j < Degree(); j++)
        {
            m_left[j] = t - knots[span - j];
            m_right[j] = knots[span + j + 1] - t;

            ScalarType saved = 0.0;

            for (std::size_t r = 0; r <= j; r++)
            {
                Ndu(j + 1, r) = m_right[r] + m_left[j - r];

                ScalarType temp = Ndu(r, j) / Ndu(j + 1, r);

                Ndu(r, j + 1) = saved + m_right[r] * temp;

                saved = m_left[j - r] * temp;
            }

            Ndu(j + 1, j + 1) = saved;
        }

        for (std::size_t j = 0; j < NbNonzeroPoles(); j++) {
            Values(0, j) = Ndu(j, Degree());
        }

        auto& a = m_a;
        auto& b = m_b;

        for (std::size_t r = 0; r < NbNonzeroPoles(); r++) {
            a[0] = 1.0;

            for (std::size_t k = 1; k < NbShapes(); k++) {
                ScalarType& value = Values(k, r);

                std::size_t rk = r - k;
                std::size_t pk = Degree() - k;

                if (r >= k) {
                    b[0] = a[0] / Ndu(pk + 1, rk);
                    value = b[0] * Ndu(rk, pk);
                }

                std::size_t j1 = r >= k - 1 ? 1 : k - r;
                std::size_t j2 = r <= pk + 1 ? k - 1 : Degree() - r;

                for (std::size_t j = j1; j <= j2; j++) {
                    b[j] = (a[j] - a[j - 1]) / Ndu(pk + 1, rk + j);
                    value += b[j] * Ndu(rk + j, pk);
                }

                if (r <= pk) {
                    b[k] = -a[k - 1] / Ndu(pk + 1, r);
                    value += b[k] * Ndu(r, pk);
                }

                std::swap(a, b);
            }
        }

        std::size_t s = Degree();

        for (std::size_t k = 1; k < NbShapes(); k++) {
            for (std::size_t j = 0; j < NbNonzeroPoles(); j++) {
                Values(k, j) *= s;
            }
            s *= Degree() - k;
        }
    }

    template <typename TKnots, typename TWeights>
    void
    ComputeAtSpan(
        const TKnots& knots,
        const std::size_t& span,
        const TWeights& weights,
        const ScalarType& t
    )
    {
        // compute B-Spline shape

        ComputeAtSpan(knots, span, t);

        // compute weighted sum

        ScalarType weightedSum{0};

        for (std::size_t i = 0; i < NbNonzeroPoles(); i++) {
            m_values[i] *= Util::Weights<TWeights>::Get(weights, i);
            weightedSum += m_values[i];
        }

        // apply weights

        for (std::size_t i = 0; i < NbNonzeroPoles(); i++) {
            m_values[i] /= weightedSum;
        }
    }
    
    template <typename TKnots>
    void
    Compute(
        const TKnots& knots,
        const ScalarType& t
    )
    {
        std::size_t span = Knots::LowerSpan(Degree(), knots, t);

        ComputeAtSpan(knots, span, t);
    }
    
    template <typename TKnots, typename TWeights>
    void
    Compute(
        const TKnots& knots,
        const TWeights& weights,
        const ScalarType& t
    )
    {
        std::size_t span = Knots::LowerSpan(Degree(), knots, t);

        ComputeAtSpan(knots, span, weights, t);
    }
};

} // namespace ANurbs
