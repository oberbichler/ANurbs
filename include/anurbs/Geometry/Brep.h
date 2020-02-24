#pragma once

#include "BrepEdge.h"
#include "BrepFace.h"
#include "BrepLoop.h"
#include "BrepTrim.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace anurbs {

class BrepFace;
class BrepLoop;
class BrepTrim;
class BrepEdge;

class Brep
{
private:    // variables
    std::vector<Ref<BrepFace>> m_faces;
    std::vector<Ref<BrepLoop>> m_loops;
    std::vector<Ref<BrepTrim>> m_trims;
    std::vector<Ref<BrepEdge>> m_edges;

public:     // methods
    size_t nb_faces() const;

    Ref<BrepFace> face(size_t index) const;

    std::vector<Ref<BrepFace>> faces();

    size_t nb_loops() const;

    Ref<BrepLoop> loop(size_t index) const;

    std::vector<Ref<BrepLoop>> loops();

    size_t nb_trims() const;

    Ref<BrepTrim> trim(size_t index) const;

    std::vector<Ref<BrepTrim>> trims();

    size_t nb_edges() const;

    Ref<BrepEdge> edge(size_t index) const;

    std::vector<Ref<BrepEdge>> edges();

public:     // serialization
    static std::string type_name();

    static Unique<Brep> load(Model& model, const Json& data);

    static void save(const Model& model, const Brep& data, Json& target);

public:     // python
    static std::string python_name();

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Brep;
        using Holder = Pointer<Brep>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            .def("nb_faces", &Type::nb_faces)
            .def("face", &Type::face, "index"_a)
            .def("faces", &Type::faces)
            .def("nb_loops", &Type::nb_loops)
            .def("loop", &Type::loop, "index"_a)
            .def("loops", &Type::loops)
            .def("nb_trims", &Type::nb_trims)
            .def("trim", &Type::trim, "index"_a)
            .def("trims", &Type::trims)
            .def("nb_edges", &Type::nb_edges)
            .def("edge", &Type::edge, "index"_a)
            .def("edges", &Type::edges)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs