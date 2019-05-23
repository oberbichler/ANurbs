#pragma once

#include "BrepEdge.h"
#include "BrepFace.h"
#include "BrepTrim.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

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
    Ref<Brep> brep();

    Ref<BrepFace> face();

    size_t nb_trims();

    Ref<BrepTrim> trim(size_t index);

    std::vector<Ref<BrepTrim>> trims();

    std::vector<Ref<BrepEdge>> edges();

public:     // serialization
    using Attributes = Attributes;

    static std::string type_name()
    {
        return "BrepLoop";
    }

    static Unique<BrepLoop> load(Model& model, const Json& data)
    {
        auto result = new_<BrepLoop>();

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
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepLoop;

        py::class_<Type>(m, "BrepLoop")
            .def("brep", &Type::brep)
            .def("face", &Type::face)
            .def("nb_trims", &Type::nb_trims)
            .def("trim", &Type::trim, "index"_a)
            .def("trims", &Type::trims)
            .def("edges", &Type::edges)
        ;
    }
};

} // namespace ANurbs