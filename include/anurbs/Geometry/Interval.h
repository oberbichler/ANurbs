#pragma once

#include "../Define.h"

#include <sstream>

namespace anurbs {

class Interval
{
private:    // variables
    double m_t0;
    double m_t1;

public:     // constructor
    Interval() : m_t0(QuietNaN), m_t1(QuietNaN)
    {
    }

    Interval(double t0, double t1) : m_t0(t0), m_t1(t1)
    {
    }

    Interval(std::pair<double, double> interval) : m_t0(interval.first),
        m_t1(interval.second)
    {
    }

public:     // methods
    double t0() const
    {
        return m_t0;
    }

    void set_t0(double value)
    {
        m_t0 = value;
    }

    double t1() const
    {
        return m_t1;
    }

    void set_t1(double value)
    {
        m_t1 = value;
    }

    double mid() const
    {
        return 0.5 * (m_t0 + m_t1);
    }

    double min() const
    {
        return std::min(m_t0, m_t1);
    }

    double max() const
    {
        return std::max(m_t0, m_t1);
    }

    double delta() const
    {
        return m_t1 - m_t0;
    }

    double length() const
    {
        return std::abs(delta());
    }

    double normalized_at(const double t) const
    {
        return (t - m_t0) / length();
    }

    double parameter_at_normalized(const double t) const
    {
        return m_t0 + delta() * t;
    }

    static double parameter_at_normalized(const double a, const double b,
        const double t)
    {
        return a + (b - a) * t;
    }

    Interval normalized_interval(const double t0, const double t1) const
    {
        double t0Normalized = normalized_at(t0);
        double t1Normalized = normalized_at(t1);

        return Interval(t0Normalized, t1Normalized);
    }

    Interval normalized_interval(const Interval interval) const
    {
        return normalized_interval(interval.m_t0, interval.m_t1);
    }

    bool contains(const double t) const
    {
        return (min() <= t) && (t <= max());
    }

    double clamp(const double value) const
    {
        return clamp(value, min(), max());
    }

    std::string to_string()
    {
        std::ostringstream oss;

        oss << "<Interval t0=" << m_t0 << " t1=" << m_t1 << ">";

        return oss.str();
    }

public:     // static methods
    static double clamp(const double value, const double min, const double max)
    {
        if (value < min) {
            return min;
        }

        if (value > max) {
            return max;
        }

        return value;
    }

    static double clamp_01(const double value)
    {
        return clamp(value, 0, 1);
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Interval;

        py::class_<Type>(m, "Interval")
            .def(py::init<double, double>())
            .def_property("t0", &Type::t0, &Type::set_t0)
            .def_property("t1", &Type::t1, &Type::set_t1)
            .def_property_readonly("min", &Type::min)
            .def_property_readonly("max", &Type::max)
            .def_property_readonly("delta", &Type::delta)
            .def_property_readonly("length", &Type::length)
            .def("normalized_at", &Type::normalized_at,
                "t"_a)
            .def("parameter_at_normalized", py::overload_cast<const double>(
                &Type::parameter_at_normalized, py::const_), "t"_a)
            .def("clamp", (double (Type::*)(const double) const) &Type::clamp,
                "t"_a)
        ;
    }
};

} // namespace anurbs