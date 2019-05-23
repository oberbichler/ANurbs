#pragma once

#include "BrepEdge.h"
#include "BrepLoop.h"
#include "BrepTrim.h"
#include "NurbsSurfaceGeometry.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

class Brep;
class BrepEdge;
class BrepLoop;
class BrepTrim;

class BrepFace
{
private:
    Ref<ANurbs::Brep> m_brep;
    std::vector<Ref<BrepLoop>> m_loops;
    Ref<NurbsSurfaceGeometry<3>> m_geometry;

public:
    Ref<ANurbs::Brep> brep() const;

    size_t nb_loops() const;

    Ref<BrepLoop> loop(size_t index) const;

    std::vector<Ref<BrepEdge>> edges() const;

    std::vector<Ref<BrepTrim>> trims() const;

    std::vector<Ref<BrepLoop>> loops() const;
    
    Ref<NurbsSurfaceGeometry<3>> surface_geometry() const;

public:     // serialization
    using Attributes = Attributes;

    static std::string type_name()
    {
        return "BrepFace";
    }

    static Unique<BrepFace> load(Model& model, const Json& data)
    {
        auto result = new_<BrepFace>();

        // Read Brep
        {
            const std::string key = data.at("Brep");
            result->m_brep = model.get_lazy<ANurbs::Brep>(key);
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

        // Read Geometry
        {
            const std::string key = data.at("Geometry");
            result->m_geometry = model.get_lazy<NurbsSurfaceGeometry<3>>(key);
        }

        return result;
    }

    static void save(const Model& model, const BrepFace& data, Json& target)
    {
        target["Brep"] = ToJson(data.m_brep);
        target["Loops"] = ToJson(data.m_loops);
        target["Geometry"] = ToJson(data.m_geometry);
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepFace;
        using Holder = Pointer<Type>;

        py::class_<Type, Holder>(m, "BrepFace")
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

} // namespace ANurbs