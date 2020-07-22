#pragma once

#include "../Define.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace anurbs {

template <Index TDimension>
class Point
{
public:     // types
    using Type = Point<TDimension>;
    using Vector = Eigen::Matrix<double, 1, TDimension>;

private:    // variables
    Vector m_location;
    std::string m_text;

public:     // constructors
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
    static std::string type_name()
    {
        return "Point" + std::to_string(TDimension) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const DataReader reader(source);

        auto data = new_<Type>();

        reader.fill_vector("location", data->m_location);

        if (reader.has("text")) {
            data->m_text = reader.read<std::string>("text");
        }

        return data;
    }

    static void save(const Model& model, const Point& data, Json& target)
    {
        target["location"] = ToJson(data.m_location);

        if (!data.m_text.empty()) {
            target["text"] = ToJson(data.m_text);
        }
    }

public:     // python
    static std::string python_name()
    {
        return "Point" + std::to_string(TDimension) + "D";
    }

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Point<TDimension>;
        using Holder = anurbs::Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            // constructors
            .def(py::init<const Vector&>(), "location"_a)
            .def(py::init<const Vector&, const std::string&>(), "location"_a,
                "text"_a)
            // properties
            .def_property("location", &Type::location, &Type::set_location)
            .def_property("text", &Type::text, &Type::set_text)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs