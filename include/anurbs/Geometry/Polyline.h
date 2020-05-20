#pragma once

#include "../Define.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace anurbs {

template <Index TDimension>
class Polyline
{
public:     // types
    using Type = Polyline<TDimension>;
    using Vector = Eigen::Matrix<double, 1, TDimension>;

private:    // variables
    std::vector<Vector> m_points;

public:     // constructors
    Polyline()
    {
    }

    Polyline(const Index nbPoints) : m_points(nbPoints)
    {
    }

    Polyline(const std::vector<Vector> points)
    : m_points(points)
    {
    }

public:     // methods
    Index nb_points() const
    {
        return length(m_points);
    }

    Vector point(const size_t index) const
    {
        return m_points[index];
    }

    void set_point(const size_t index,const Vector& value)
    {
        m_points[index] = value;
    }

public:     // serialization
    static std::string type_name()
    {
        return "Polyline" + std::to_string(TDimension) + "D";
    }

    static Unique<Polyline<TDimension>> load(Model& model, const Json& data)
    {
        auto result = new_<Polyline<TDimension>>();

        // load Points
        {
            const auto points = data.at("points");

            result->m_points.resize(points.size());

            for (Index i = 0; i < length(points); i++) {
                result->m_points[i] = points[i];
            }
        }

        return result;
    }

    static void save(const Model& model, const Polyline& data, Json& target)
    {
        target["points"] = ToJson(data.m_points);
    }

public:     // python
    static std::string python_name()
    {
        return "Polyline" + std::to_string(TDimension) + "D";
    }

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Holder = anurbs::Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            .def(py::init<Index>(), "nb_points"_a)
            .def(py::init<std::vector<Vector>>(), "points"_a)
            .def("nb_points", &Type::nb_points)
            .def("point", &Type::point, "index"_a)
            .def("set_point", &Type::set_point, "index"_a, "value"_a)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs