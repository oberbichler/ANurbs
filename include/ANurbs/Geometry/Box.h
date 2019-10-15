#pragma once

#include "../Define.h"

namespace ANurbs {

template <Index TDimension>
class Box
{
private:    // types
    using Type = Box<TDimension>;
    using Vector = linear_algebra::Vector<TDimension>;

private:    // variables
    Vector m_min;
    Vector m_max;

public:     // constructor
    Box(Vector a, Vector b)
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

public:     // static methods
    static constexpr Index dimension()
    {
        return TDimension;
    }

public:     // methods
    Vector min() const
    {
        return m_min;
    }

    Vector max() const
    {
        return m_max;
    }

public:     // serialization
    static std::string type_name()
    {
        return "Box" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const auto min = source.at("Min");
        const auto max = source.at("Max");

        auto result = new_<Type>(min, max);

        return result;
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        target["Min"] = data.min();
        target["Max"] = data.max();
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Holder = Pointer<Type>;

        const std::string name = Type::type_name();

        py::class_<Type, Holder>(m, name.c_str())
            // constructors
            .def(py::init<Vector, Vector>(), "a"_a, "b"_a)
            // read-only properties
            .def_property_readonly("min", &Type::min)
            .def_property_readonly("max", &Type::max)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace ANurbs