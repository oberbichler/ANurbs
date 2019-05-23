#pragma once

#include "BrepFace.h"
#include "BrepLoop.h"
#include "BrepTrim.h"
#include "BrepEdge.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace ANurbs {

class Brep
{
private:
    std::vector<Ref<BrepFace>> m_faces;
    std::vector<Ref<BrepLoop>> m_loops;
    std::vector<Ref<BrepTrim>> m_trims;
    std::vector<Ref<BrepEdge>> m_edges;

public:
    size_t nb_faces() const
    {
        return m_faces.size();
    }

    Ref<BrepFace> face(size_t index) const
    {
        return m_faces[index];
    }

    std::vector<Ref<BrepFace>> faces()
    {
        return m_faces;
    }

    size_t nb_loops() const
    {
        return m_loops.size();
    }

    Ref<BrepLoop> loop(size_t index) const
    {
        return m_loops[index];
    }

    std::vector<Ref<BrepLoop>> loops()
    {
        return m_loops;
    }

    size_t nb_trims() const
    {
        return m_trims.size();
    }

    Ref<BrepTrim> trim(size_t index) const
    {
        return m_trims[index];
    }

    std::vector<Ref<BrepTrim>> trims()
    {
        return m_trims;
    }

    size_t nb_edges() const
    {
        return m_edges.size();
    }

    Ref<BrepEdge> edge(size_t index) const
    {
        return m_edges[index];
    }

    std::vector<Ref<BrepEdge>> edges()
    {
        return m_edges;
    }

public:     // serialization
    using Attributes = CadAttributes;

    static std::string type_name()
    {
        return "Brep";
    }

    static Unique<Brep> load(Model& model, const Json& data)
    {
        auto result = new_<Brep>();

        // Read faces
        {
            const auto faces = data.at("Faces");

            result->m_faces.resize(faces.size());

            for (size_t i = 0; i < faces.size(); i++) {
                const std::string key = faces[i];
                result->m_faces[i] = model.get_lazy<BrepFace>(key);
            }
        }

        // Read loops
        {
            const auto loops = data.at("Loops");

            result->m_loops.resize(loops.size());

            for (size_t i = 0; i < loops.size(); i++) {
                const std::string key = loops[i];
                result->m_loops[i] = model.get_lazy<BrepLoop>(key);
            }
        }

        // Read trims
        {
            const auto trims = data.at("Trims");

            result->m_trims.resize(trims.size());

            for (size_t i = 0; i < trims.size(); i++) {
                const std::string key = trims[i];
                result->m_trims[i] = model.get_lazy<BrepTrim>(key);
            }
        }

        // Read edges
        {
            const auto edges = data.at("Edges");

            result->m_edges.resize(edges.size());

            for (size_t i = 0; i < edges.size(); i++) {
                const std::string key = edges[i];
                result->m_edges[i] = model.get_lazy<BrepEdge>(key);
            }
        }

        return result;
    }

    static void save(const Model& model, const Brep& data, Json& target)
    {
        target["Faces"] = ToJson(data.m_faces);
        target["Loops"] = ToJson(data.m_loops);
        target["Trims"] = ToJson(data.m_trims);
        target["Edges"] = ToJson(data.m_edges);
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Brep;

        py::class_<Type>(m, "Brep")
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

} // namespace ANurbs