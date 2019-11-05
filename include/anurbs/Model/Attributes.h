#pragma once

#include "../Define.h"

#include <set>
#include <string>
#include <map>

namespace anurbs {

class Attributes
{
private:    // variables
    std::set<std::string> m_tags;
    std::map<std::string, std::string> m_userStrings;
    std::string m_layer;
    std::string m_color;
    std::string m_arrowhead;

public:     // methods
    bool has_tag(const std::string& name)
    {
        return (m_tags.find(name) != m_tags.end());
    }

    void add_tag(const std::string& name)
    {
        m_tags.insert(name);
    }

    void remove_tag(const std::string& name)
    {
        m_tags.erase(name);
    }

    bool has_user_string(const std::string& name)
    {
        return (m_userStrings.find(name) != m_userStrings.end());
    }

    std::string user_string(const std::string& name)
    {
        return m_userStrings.at(name);
    }

    void set_user_string(const std::string& name, const std::string& value)
    {
        m_userStrings.insert({name, value});
    }

    void remove_user_string(const std::string& name)
    {
        m_userStrings.erase(name);
    }

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

public:     // serialization
    template <typename TModel, typename TSource>
    void load(TModel& model, const TSource& source)
    {
        m_tags = source.value("Tags", std::set<std::string>());
        m_userStrings = source.value("UserStrings",
            std::map<std::string, std::string>());
        m_layer = source.value("Layer", "");
        m_color = source.value("Color", "");
        m_arrowhead = source.value("Arrowhead", "");
    }

    template <typename TModel, typename TTarget>
    void save(const TModel& model, TTarget& target) const
    {
        if (m_tags.size() != 0) {
            target["Tags"] = m_tags;
        }

        if (m_userStrings.size() != 0) {
            target["UserStrings"] = m_userStrings;
        }

        if (!m_layer.empty()) {
            target["Layer"] = m_layer;
        }
        
        if (!m_color.empty()) {
            target["Color"] = m_color;
        }
        
        if (!m_arrowhead.empty()) {
            target["Arrowhead"] = m_arrowhead;
        }
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Attributes;

        py::class_<Type, Pointer<Type>>(m, "Attributes")
            // methods
            .def("add_tag", &Type::add_tag, "name"_a)
            .def("arrowhead", &Type::arrowhead)
            .def("color", &Type::color)
            .def("has_tag", &Type::has_tag, "name"_a)
            .def("has_user_string", &Type::has_user_string, "name"_a)
            .def("layer", &Type::layer)
            .def("remove_tag", &Type::remove_tag, "name"_a)
            .def("remove_user_string", &Type::remove_user_string, "name"_a)
            .def("set_arrowhead", &Type::set_arrowhead, "value"_a)
            .def("set_color", &Type::set_color, "value"_a)
            .def("set_layer", &Type::set_layer, "value"_a)
            .def("set_user_string", &Type::set_user_string, "name"_a, "value"_a)
            .def("user_string", &Type::user_string, "name"_a)
        ;
    }
};

} // namespace anurbs