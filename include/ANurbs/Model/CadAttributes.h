#pragma once

#include "../Define.h"

#include "Attributes.h"

namespace ANurbs {

class CadAttributes : public Attributes
{
private:
    std::string m_layer;
    std::string m_color;
    std::string m_arrowhead;

public:
    template <typename TModel, typename TSource>
    void load(TModel& model, const TSource& source)
    {
        Attributes::load(model, source);

        m_layer = source.value("layer", "");
        m_color = source.value("color", "");
        m_arrowhead = source.value("arrowhead", "");
    }

    template <typename TModel, typename TTarget>
    void save(const TModel& model, TTarget& target) const
    {
        Attributes::save(model, target);

        if (!m_layer.empty()) {
            target["layer"] = m_layer;
        }
        
        if (!m_color.empty()) {
            target["color"] = m_color;
        }
        
        if (!m_arrowhead.empty()) {
            target["arrowhead"] = m_arrowhead;
        }
    }

public:
    std::string layer() const
    {
        return m_layer;
    }

    void set_layer(const std::string& value)
    {
        m_layer = value;
    }

    std::string color() const
    {
        return m_color;
    }

    void set_color(const std::string& value)
    {
        m_color = value;
    }

    std::string arrowhead() const
    {
        return m_arrowhead;
    }

    void set_arrowhead(const std::string& value)
    {
        m_arrowhead = value;
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = CadAttributes;
        using Base = Attributes;
        using Holder = Pointer<Type>;

        py::class_<Type, Base, Holder>(m, "CadAttributes")
            .def("arrowhead", &Type::arrowhead)
            .def("color", &Type::color)
            .def("layer", &Type::layer)
            .def("set_arrowhead", &Type::set_arrowhead, "value"_a)
            .def("set_color", &Type::set_color, "value"_a)
            .def("set_layer", &Type::set_layer, "value"_a)
        ;
    }
};

} // namespace ANurbs