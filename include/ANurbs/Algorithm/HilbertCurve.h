#pragma once

#include "../Define.h"

#include <bitset>

namespace ANurbs {

template <Index TDimension>
class HilbertCurve
{
private:    // types
    using Vector = linear_algebra::Vector<TDimension>;
    using VectorU = Eigen::Matrix<size_t, 1, TDimension>;

private:    // methods
    static size_t log2(size_t value) noexcept
    {
        size_t result = 0;

        while (value >>= 1) {
            result += 1;
        }

        return result;
    }

    static size_t rotl(const size_t x, const Index d) noexcept
    {
        std::bitset<TDimension> bits(x);

        const Index c = d % TDimension >= 0 ? d % TDimension
                                            : d % TDimension + TDimension;

        return (bits << c | bits >> (TDimension - c)).to_ulong();
    }

    static size_t rotr(const size_t x, const Index d) noexcept
    {
        std::bitset<TDimension> bits(x);

        const Index c = d % TDimension >= 0 ? d % TDimension
                                            : d % TDimension + TDimension;

        return (bits >> c | bits << (TDimension - c)).to_ulong();
    }

    static size_t bit(const size_t x, const size_t i) noexcept
    {
        return x >> i & 1;
    }

    static size_t gc(const size_t i) noexcept
    {
        return i ^ (i >> 1);
    }

    static size_t e(const size_t i) noexcept
    {
        return i == 0 ? 0 : gc(((i - 1) >> 1) << 1);
    }

    static size_t inverse_gc(const size_t gc) noexcept
    {
        size_t i = gc;
        size_t j = 1;

        while (j < TDimension) {
            i = i ^ (gc >> j);
            j = j + 1;
        }

        return i;
    }

    static size_t g(const size_t i) noexcept
    {
        return log2(gc(i) ^ gc(i + 1));
    }

    static size_t d(const size_t i) noexcept
    {
        if (i == 0) {
            return 0;
        } else if (i % 2 == 0) {
            return g(i - 1) % TDimension;
        } else {
            return g(i) % TDimension;
        }
    }

    static size_t t(const size_t e, const Index d, const size_t b) noexcept
    {
        return rotr(b ^ e, d + 1);
    }

    static size_t inverse_t(const size_t e, const Index d, const size_t b) noexcept
    {
        return t(rotr(e, d + 1), TDimension - d - 2, b);
    }

    static size_t project(const size_t m, const VectorU p) noexcept
    {
        size_t h = 0;
        size_t ve = 0;
        size_t vd = 0;

        for (Index i = m - 1; i > -1; i--) {
            size_t s = 0;

            for (Index j = 0; j < TDimension; j++) {
                s += bit(p[j], i) << j;
            }

            const size_t l = t(ve, vd, s);
            const size_t w = inverse_gc(l);

            ve = ve ^ (rotl(e(w), vd + 1));
            vd = (vd + d(w) + 1) % TDimension;
            h = (h << TDimension) | w;
        }

        return h;
    }

    static VectorU evaluate(const size_t m, const size_t h) noexcept
    {
        size_t ve = 0;
        size_t vd = 0;

        VectorU p = VectorU::Zero();

        for (Index i = m - 1; i > -1; i--) {
            size_t w = 0;

            for (Index j = 0; j < TDimension; j++) {
                w += bit(h, i * TDimension + j) << j;
            }

            const size_t l = inverse_t(ve, vd, gc(w));

            for (Index j = 0; j < TDimension; j++) {
                p[j] += bit(l, j) << i;
            }

            ve = ve ^ rotl(e(w), vd + 1);
            vd = (vd + d(w) + 1) % TDimension;
        }

        return p;
    }

private:    // variables
    Vector m_min;
    Vector m_max;
    size_t m_m = 4 * sizeof(size_t) / TDimension;   // TODO: Use full range

public:     // constructor
    HilbertCurve(const Vector a, const Vector b)
    {
        for (Index i = 0; i < TDimension; i++) {
            if (a[i] < b[i]) {
                m_min[i] = a[i];
                m_max[i] = b[i];
            } else {
                m_min[i] = b[i];
                m_max[i] = a[i];
            }
        }
    }

public:     // methods
    Vector point_at(const size_t h) const
    {
        VectorU p = evaluate(m_m, h);

        Vector result;

        for (Index i = 0; i < TDimension; i++) {
            result[i] = m_min[i] + p[i] * (m_max[i] - m_min[i]) / (1 << m_m);
        }

        return result;
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = HilbertCurve<TDimension>;
        using Holder = Pointer<Type>;

        const std::string name = "HilbertCurve" + std::to_string(TDimension) + "D";
        
        py::class_<Type, Holder>(m, name.c_str())
            // private static methods
            .def_static("_bit", &Type::bit)
            .def_static("_e", &Type::e)
            .def_static("_g", &Type::g)
            .def_static("_gc", &Type::gc)
            .def_static("_inverse_gc", &Type::inverse_gc)
            .def_static("_inverse_t", &Type::inverse_t)
            .def_static("_rotl", &Type::rotl)
            .def_static("_rotr", &Type::rotr)
            .def_static("_t", &Type::t)
            .def_static("_evaluate", &Type::evaluate)
            .def_static("_project", &Type::project)
            // constructor
            .def(py::init<Vector, Vector>())
            .def("point_at", &Type::point_at)
        ;
    }
};

} // namespace ANurbs