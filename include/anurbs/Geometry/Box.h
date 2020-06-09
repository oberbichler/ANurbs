#pragma once

#include "../Define.h"

#include "../Model/DataReader.h"
#include "../Model/DataWriter.h"

namespace anurbs {

template <Index TDimension>
class Box
{
private:    // types
    using Type = Box<TDimension>;
    using Vector = Eigen::Matrix<double, 1, TDimension>;

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

    Eigen::Ref<Vector> min()
    {
        return m_min;
    }

    Vector max() const
    {
        return m_max;
    }

    Eigen::Ref<Vector> max()
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
        const DataReader reader(source);

        Vector min, max;

        reader.fill_vector("min", min);
        reader.fill_vector("max", max);

        return new_<Type>(min, max);
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        DataWriter writer(target);

        writer.write_vector("min", data.min());
        writer.write_vector("max", data.max());
    }

public:     // python
    static std::string python_name()
    {
        return "Box" + std::to_string(dimension()) + "D";
    }

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            // constructors
            .def(py::init<Vector, Vector>(), "a"_a, "b"_a)
            // read-only properties
            .def_property_readonly("min", py::overload_cast<>(&Type::min))
            .def_property_readonly("max", py::overload_cast<>(&Type::max))
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs