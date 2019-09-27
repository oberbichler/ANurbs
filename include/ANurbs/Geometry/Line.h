#pragma once

#include "../Define.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace ANurbs {

template <Index TDimension>
class Line
{
public:
    using Vector = linear_algebra::Vector<TDimension>;

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

public:     // serialization
    static std::string type_name()
    {
        return "Line" + std::to_string(TDimension) + "D";
    }

    static Unique<Line> load(Model& model, const Json& data)
    {
        auto result = new_<Line>();

        result->m_a = data.at("A");
        result->m_b = data.at("B");

        return result;
    }

    static void save(const Model& model, const Line& data, Json& target)
    {
        target["A"] = ToJson(data.m_a);
        target["B"] = ToJson(data.m_b);
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Line<TDimension>;
        using Holder = ANurbs::Pointer<Type>;

        const std::string name = Type::type_name();

        py::class_<Type, Holder>(m, name.c_str())
            // constructors
            .def(py::init<const Vector&, const Vector&>(), "a"_a, "b"_a)
            // properties
            .def_property("a", &Type::a, &Type::set_a)
            .def_property("b", &Type::b, &Type::set_b)
        ;

        m.def("add", [](Model& model, Holder data) { model.add<Type>(data); } );

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace ANurbs