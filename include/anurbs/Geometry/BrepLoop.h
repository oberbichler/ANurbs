#pragma once

#include "Brep.h"
#include "BrepFace.h"
#include "BrepTrim.h"
#include "BrepEdge.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace anurbs {

class Brep;
class BrepEdge;
class BrepFace;
class BrepTrim;

class BrepLoop
{
private:
    Ref<BrepFace> m_face;
    std::vector<Ref<BrepTrim>> m_trims;

public:
    Ref<Brep> brep() const;

    Ref<BrepFace> face() const;

    size_t nb_trims() const;

    Ref<BrepTrim> trim(size_t index) const;

    std::vector<Ref<BrepTrim>> trims() const;

    std::vector<Ref<BrepEdge>> edges() const;

public:     // serialization
    static std::string type_name();

    static Unique<BrepLoop> load(Model& model, const Json& data);

    static void save(const Model& model, const BrepLoop& data, Json& target);

public:     // python
    static std::string python_name();

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepLoop;
        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            // read-only properties
            .def_property_readonly("brep", &Type::brep)
            .def_property_readonly("edges", &Type::edges)
            .def_property_readonly("face", &Type::face)
            .def_property_readonly("nb_trims", &Type::nb_trims)
            .def_property_readonly("trims", &Type::trims)
            // methods
            .def("trim", &Type::trim, "index"_a)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs