#pragma once

#include "../Define.h"

#include "BrepFace.h"
#include "BrepLoop.h"
#include "BrepTrim.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace ANurbs {

class Brep;
class BrepFace;
class BrepLoop;
class BrepTrim;

class BrepEdge
{
private:    // variables
    std::vector<Ref<BrepTrim>> m_trims;

public:     // methods
    std::vector<Ref<BrepFace>> faces();

    std::vector<Ref<BrepLoop>> loops();

    size_t nb_trims();

    Ref<BrepTrim> trim(size_t index);

    std::vector<Ref<BrepTrim>> trims();

public:     // serialization
    using Attributes = Attributes;

    static std::string type_name()
    {
        return "BrepEdge";
    }

    static Unique<BrepEdge> load(Model& model, const Json& data)
    {
        auto result = new_<BrepEdge>();

        // Read trims
        {
            const auto trims = data.at("trims");

            result->m_trims.resize(trims.size());

            for (size_t i = 0; i < trims.size(); i++) {
                const std::string key = trims[i];
                result->m_trims[i] = model.get_lazy<BrepTrim>(key);
            }
        }

        return result;
    }

    static void save(const Model& model, const BrepEdge& data, Json& target)
    {
        target["Trims"] = ToJson(data.m_trims);
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepEdge;

        py::class_<Type>(m, "BrepEdge")
            .def("nb_trims", &Type::nb_trims)
            .def("trim", &Type::trim, "index"_a)
            .def("trims", &Type::trims)
            .def("faces", &Type::faces)
            .def("loops", &Type::loops)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace ANurbs