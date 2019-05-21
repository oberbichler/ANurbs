#pragma once

#include "../Define.h"

#include "../Model/Define.h"
#include "../Model/CadAttributes.h"
#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace ANurbs {

template <int TDimension>
class Point
{
public:
    using Vector = Vector<TDimension>;

private:
    Vector m_location;
    std::string m_text;

public:
    using Type = Point<TDimension>;

    Point()
    {
    }

    Point(const Vector& location)
        : m_location(location), m_text()
    {
    }

    Point(const Vector& location, const std::string& text)
        : m_location(location), m_text(text)
    {
    }

    Vector location() const
    {
        return m_location;
    }

    void set_location(const Vector& value)
    {
        m_location = value;
    }

    std::string text() const
    {
        return m_text;
    }

    void set_text(const std::string& value)
    {
        m_text = value;
    }

public:     // serialization
    using Attributes = CadAttributes;

    static std::string type_name()
    {
        return "Point" + std::to_string(TDimension) + "D";
    }

    static Unique<Type>
    load(
        Model& model,
        const Json& source)
    {
        auto data = new_<Point<TDimension>>();

        // data->m_location = source.at("Location");
        // data->m_text = source.at("Text");

        return data;
    }

    static void save(const Model& model, const Point& data, Json& target)
    {
        // target["Location"] = ToJson(m_location);
        // target["Text"] = ToJson(m_text);
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Point<TDimension>;
        using Holder = ANurbs::Pointer<Type>;

        const std::string name = Type::type_name();

        py::class_<Type, Holder>(m, name.c_str())
            .def(py::init<const Vector&>(), "location"_a)
            .def("location", &Type::location)
        ;
        
        // RegisterDataType<Type>(m, model, name);
    }
};

} // namespace ANurbs