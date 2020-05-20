#pragma once

#include "../Define.h"

#include "BrepFace.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace anurbs {

template <Index TDimension>
class BrepFaceField
{
public:     // types
    using Vector = Eigen::Matrix<double, 1, TDimension>;

private:    // variables
    Ref<BrepFace> m_face;
    std::vector<Vector> m_values;

public:     // constructors
    BrepFaceField(Ref<BrepFace> face) : m_face(face),
        m_values(face->surface_geometry()->nb_poles())
    { }

public:     // methods
    static constexpr Index dimension()
    {
        return TDimension;
    }

    Ref<BrepFace> face() const
    {
        return m_face;
    }

    const std::vector<Vector>& values() const
    {
        return m_values;
    }

    Vector value(Index index) const
    {
        return m_values[index];
    }

    void set_value(Index index, const Vector& value)
    {
        m_values[index] = value;
    }

public:     // serialization
    static std::string type_name()
    {
        return "BrepFaceField" + std::to_string(dimension()) + "D";
    }

    static Unique<BrepFaceField> load(Model& model, const Json& data)
    {
        auto result = new_<BrepFaceField>();

        // Read Face
        {
            const std::string key = data.at("face");
            result->m_face = model.get_lazy<BrepFace>(key);
        }

        // Read Values
        {
            const auto values = data.at("values");

            result->m_values.resize(values.size());

            for (size_t i = 0; i < values.size(); i++) {
                result->m_values[i] = values[i];
            }
        }

        return result;
    }

    void save(const Model& model, Json& data) const
    {
        data["face"] = ToJson(m_face);
        data["values"] = ToJson(m_values);
    }
    
public:     // python
    static std::string python_name()
    {
        return "BrepFaceField" + std::to_string(dimension()) + "D";
    }

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepFaceField<TDimension>;
        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            // constructors
            .def(py::init<Ref<BrepFace>>(), "face"_a)
            // read-only properties
            .def_property_readonly("face", &Type::face)
            .def_property_readonly("values", &Type::values)
            // methods
            .def("set_value", &Type::set_value, "index"_a, "value"_a)
            .def("value", &Type::value, "index"_a)
        ;
    }
};

} // namespace anurbs