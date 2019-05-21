#pragma once

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace ANurbs {

class BrepTrim;

class BrepEdge
{
private:
    std::vector<Ref<BrepTrim>> m_trims;

public:
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
        return "BrepEdge";
    }

    static Unique<BrepEdge> load(Model& model, const Json& data)
    {
        auto result = New<BrepEdge>();

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

    static void save(const Model& model, const BrepEdge& data, Json& target)
    {
        target["Trims"] = ToJson(data.m_trims);
    }

public:     // python
    template <typename TModule, typename TModel>
    static void register_python(TModule& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepEdge;

        py::class_<Type>(m, "BrepEdge")
            .def("NbTrims", &Type::nb_trims)
            .def("Trim", &Type::trim, "index"_a)
            .def("trims", &Type::trims)
            // .def("loops", [](Type& self) -> std::vector<Ref<BrepLoop>> {
            //     std::vector<Ref<BrepLoop>> loops(self.nb_trims());
            //     for (size_t i = 0; i < loops.size(); i++) {
            //         loops[i] = self.trim(i)->loop();
            //     }
            //     return loops;
            // })
            // .def("faces", [](Type& self) -> std::vector<Ref<BrepFace>> {
            //     std::vector<Ref<BrepFace>> faces(self.nb_trims());
            //     for (size_t i = 0; i < faces.size(); i++) {
            //         faces[i] = self.trim(i)->loop()->face();
            //     }
            //     return faces;
            // })
        ;
    }
};

} // namespace ANurbs