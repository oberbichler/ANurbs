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
    Ref<ANurbs::Brep> Brep();

    size_t nb_loops();

    Ref<BrepLoop> loop(size_t index);

    std::vector<Ref<BrepLoop>> loops();
    
    Ref<NurbsSurfaceGeometry<3>> Geometry();

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
            result->m_brep = model.GetLazy<ANurbs::Brep>(key);
        }

        // Read loops
        {
            const auto loops = data.at("loops");

            result->m_loops.resize(loops.size());

            for (size_t i = 0; i < loops.size(); i++) {
                const std::string key = loops[i];
                result->m_loops[i] = model.GetLazy<BrepLoop>(key);
            }
        }

        // Read Geometry
        {
            const std::string key = data.at("Geometry");
            result->m_geometry = model.GetLazy<NurbsSurfaceGeometry<3>>(key);
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

        py::class_<Type>(m, "BrepFace")
            // .def("Brep", &Type::Brep)
            // .def("NbLoops", &Type::nb_loops)
            // .def("Loop", &Type::loop, "index"_a)
            // .def("loops", &Type::loops)
            // .def("trims", [](Type& self) -> std::vector<Ref<BrepTrim>> {
            //     std::vector<Ref<BrepTrim>> trims;
            //     for (size_t i = 0; i < self.nb_loops(); i++) {
            //         auto loop = self.loop(i);
            //         for (size_t j = 0; j < loop->nb_trims(); j++) {
            //             trims.push_back(loop->trim(j));
            //         }
            //     }
            //     return trims;
            // })
            // .def("edges", [](Type& self) -> std::vector<Ref<BrepEdge>> {
            //     std::vector<Ref<BrepEdge>> edges;
            //     for (size_t i = 0; i < self.nb_loops(); i++) {
            //         auto loop = self.loop(i);
            //         for (size_t j = 0; j < loop->nb_trims(); j++) {
            //             edges.push_back(loop->trim(j)->edge());
            //         }
            //     }
            //     return edges;
            // })
            // .def("Geometry", &Type::Geometry)
            // .def("UntrimmedSurface", [](Type& self) {
            //     return new_<Surface<NurbsSurfaceGeometry<Vector>>>(
            //         self.Geometry().Data());
            // })
        ;
    }
};

} // namespace ANurbs