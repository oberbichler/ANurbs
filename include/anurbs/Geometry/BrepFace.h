#pragma once

#include "Brep.h"
#include "BrepLoop.h"
#include "BrepTrim.h"
#include "BrepEdge.h"
#include "NurbsSurfaceGeometry.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace anurbs {

class Brep;
class BrepEdge;
class BrepLoop;
class BrepTrim;

class BrepFace
{
private:
    Ref<anurbs::Brep> m_brep;
    std::vector<Ref<BrepLoop>> m_loops;
    Ref<NurbsSurfaceGeometry<3>> m_geometry;

public:
    Ref<anurbs::Brep> brep() const;

    size_t nb_loops() const;

    Ref<BrepLoop> loop(size_t index) const;

    std::vector<Ref<BrepEdge>> edges() const;

    std::vector<Ref<BrepTrim>> trims() const;

    std::vector<Ref<BrepLoop>> loops() const;
    
    Ref<NurbsSurfaceGeometry<3>> surface_geometry() const;

public:     // serialization
    static std::string type_name();

    static Unique<BrepFace> load(Model& model, const Json& data);

    static void save(const Model& model, const BrepFace& data, Json& target);

public:     // python
    static std::string python_name();

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepFace;
        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            // read-only properties
            .def_property_readonly("brep", &Type::brep)
            .def_property_readonly("edges", &Type::edges)
            .def_property_readonly("loops", &Type::loops)
            .def_property_readonly("nb_loops", &Type::nb_loops)
            .def_property_readonly("surface_geometry", &Type::surface_geometry)
            .def_property_readonly("trims", &Type::trims)
            // methods
            .def("loop", &Type::loop, "index"_a)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs