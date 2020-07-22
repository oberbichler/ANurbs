#pragma once

#include "../Define.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace anurbs {

template <Index TDimension>
class Line
{
public:
    using Vector = Eigen::Matrix<double, 1, TDimension>;

private:
    Vector m_a;
    Vector m_b;

public:
    using Type = Line<TDimension>;

    Line()
    {
    }
    
    Line(const Vector& a, const Vector& b) : m_a(a), m_b(b)
    {
    }

    Vector a() const
    {
        return m_a;
    }

    void set_a(const Vector& value)
    {
        m_a = value;
    }

    Vector b() const
    {
        return m_b;
    }

    void set_b(const Vector& value)
    {
        m_b = value;
    }

    static std::pair<double, Vector> closest_point(const Vector& point, const Vector& line_a, const Vector& line_b, bool is_infinite=false)
    {
        const Vector v = line_b - line_a;
        const double l = v.squaredNorm();

        if (l < 1e-14) {
            return {0.0, line_a};
        }

        const Vector o = line_a;
        const Vector r = v * (1.0 / l);
        const double t = (point - o).dot(r);

        if (!is_infinite && t < 0) {
            return {0.0, line_a};
        }
        
        if (!is_infinite && t > 1) {
            return {1.0, line_b};
        }

        const Vector closest_point = line_a + t * (line_b - line_a);

        return {t, closest_point};
    }

public:     // serialization
    static std::string type_name()
    {
        return "Line" + std::to_string(TDimension) + "D";
    }

    static Unique<Line> load(Model& model, const Json& data)
    {
        auto result = new_<Line>();

        result->m_a = data.at("a");
        result->m_b = data.at("b");

        return result;
    }

    static void save(const Model& model, const Line& data, Json& target)
    {
        target["a"] = ToJson(data.m_a);
        target["b"] = ToJson(data.m_b);
    }

public:     // python
    static std::string python_name()
    {
        return "Line" + std::to_string(TDimension) + "D";
    }

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Line<TDimension>;
        using Holder = anurbs::Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            // constructors
            .def(py::init<const Vector&, const Vector&>(), "a"_a, "b"_a)
            // static methods
            .def_static("closest_point", &Type::closest_point, "point"_a, "line_a"_a, "line_b"_a, "is_infinite"_a=false)
            // properties
            .def_property("a", &Type::a, &Type::set_a)
            .def_property("b", &Type::b, &Type::set_b)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs