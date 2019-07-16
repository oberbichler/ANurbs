#pragma once

#include "../Define.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace ANurbs {

template <int TDimension>
class Polyline
{
public:     // types
    using Type = Polyline<TDimension>;
    using Vector = Vector<TDimension>;

private:    // variables
    std::vector<Vector> m_points;

public:     // constructors
    Polyline()
    {
    }
    
    Polyline(const int nbPoints) : m_points(nbPoints)
    {
    }

public:     // methods
    size_t nb_points() const
    {
        return m_points.size();
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
            const auto points = data.at("Points");

            result->m_points.resize(points.size());

            for (size_t i = 0; i < points.size(); i++) {
                result->m_points[i] = points[i];
            }
        }

        return result;
    }

    static void save(const Model& model, const Polyline& data, Json& target)
    {
        target["Points"] = ToJson(data.m_points);
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Polyline<TDimension>;
        using Holder = ANurbs::Pointer<Type>;

        const std::string name = Type::type_name();

        py::class_<Type, Holder>(m, name.c_str())
            .def(py::init<int>(), "nbPoints"_a)
            .def("nb_points", &Type::nb_points)
            .def("point", &Type::point, "index"_a)
            .def("set_point", &Type::set_point, "index"_a, "value"_a)
        ;
    }
};

} // namespace ANurbs