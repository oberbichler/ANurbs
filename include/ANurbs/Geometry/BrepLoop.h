#pragma once

#include "BrepTrim.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

class BrepFace;

class BrepLoop
{
private:
    Ref<BrepFace> m_face;
    std::vector<Ref<BrepTrim>> m_trims;

public:
    Ref<BrepFace> face()
    {
        return m_face;
    }

    size_t nb_trims()
    {
        return m_trims.size();
    }

    Ref<BrepTrim> trim(size_t index)
    {
        return m_trims[index];
    }

    std::vector<Ref<BrepTrim>> trims()
    {
        return m_trims;
    }

public:     // serialization
    using Attributes = Attributes;

    static std::string type_name()
    {
        return "BrepLoop";
    }

    static Unique<BrepLoop> load(Model& model, const Json& data)
    {
        auto result = New<BrepLoop>();

        // Read Face
        {
            const std::string key = data.at("Face");
            result->m_face = model.GetLazy<BrepFace>(key);
        }

        // Read trims
        {
            const auto trims = data.at("trims");

            result->m_trims.resize(trims.size());

            for (size_t i = 0; i < trims.size(); i++) {
                const std::string key = trims[i];
                result->m_trims[i] = model.GetLazy<BrepTrim>(key);
            }
        }

        return result;
    }

    static void save(const Model& model, const BrepLoop& data, Json& target)
    {
        target["Face"] = ToJson(data.m_face);
        target["Trims"] = ToJson(data.m_trims);
    }

public:     // python
    template <typename TModule, typename TModel>
    static void register_python(TModule& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepLoop;

        py::class_<Type>(m, "BrepLoop")
            // .def("Brep", [](Type& self) -> Ref<Brep> {
            //     return self.face()->Brep();
            // })
            // .def("Face", &Type::face)
            // .def("NbTrims", &Type::nb_trims)
            // .def("Trim", &Type::trim, "index"_a)
            // .def("trims", &Type::trims)
            // .def("edges", [](Type& self) -> std::vector<Ref<BrepEdge>> {
            //     std::vector<Ref<BrepEdge>> edges(self.nb_trims());
            //     for (size_t i = 0; i < edges.size(); i++) {
            //         edges[i] = self.trim(i)->edge();
            //     }
            //     return edges;
            // })
        ;
    }
};

} // namespace ANurbs