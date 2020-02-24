#pragma once

#include "../Define.h"

#include "Brep.h"
#include "BrepFace.h"
#include "BrepLoop.h"
#include "BrepTrim.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace anurbs {

class Brep;
class BrepFace;
class BrepLoop;
class BrepTrim;

class BrepEdge
{
private:    // variables
    std::vector<Ref<BrepTrim>> m_trims;

public:     // methods
    std::vector<Ref<BrepFace>> faces() const;

    std::vector<Ref<BrepLoop>> loops() const;

    size_t nb_trims() const;

    Ref<BrepTrim> trim(size_t index) const;

    std::vector<Ref<BrepTrim>> trims() const;

public:     // serialization
    static std::string type_name();

    static Unique<BrepEdge> load(Model& model, const Json& data);

    static void save(const Model& model, const BrepEdge& data, Json& target);

public:     // python
    static std::string python_name();

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepEdge;
        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            // read-only properties
            .def_property_readonly("faces", &Type::faces)
            .def_property_readonly("loops", &Type::loops)
            .def_property_readonly("nb_trims", &Type::nb_trims)
            .def_property_readonly("trims", &Type::trims)
            // methods
            .def("trim", &Type::trim, "index"_a)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs