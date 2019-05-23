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
    std::vector<Ref<BrepFace>> faces() const;

    std::vector<Ref<BrepLoop>> loops() const;

    size_t nb_trims() const;

    Ref<BrepTrim> trim(size_t index) const;

    std::vector<Ref<BrepTrim>> trims() const;

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
            const auto trims = data.at("Trims");

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
        using Holder = Pointer<Type>;

        py::class_<Type, Holder>(m, "BrepEdge")
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

} // namespace ANurbs