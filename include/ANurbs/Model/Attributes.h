#pragma once

#include "../Define.h"

#include <set>
#include <string>
#include <map>

namespace ANurbs {

class Attributes
{
private:
    std::set<std::string> m_tags;
    std::map<std::string, std::string> m_userStrings;

public:
    template <typename TModel, typename TSource>
    void load(TModel& model, const TSource& source)
    {
        m_tags = source.value("Tags", std::set<std::string>());
        m_userStrings = source.value("UserStrings",
            std::map<std::string, std::string>());
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
    }

public:
    bool HasTag(const std::string& name)
    {
        return (m_tags.find(name) != m_tags.end());
    }

    void AddTag(const std::string& name)
    {
        m_tags.insert(name);
    }

    void RemoveTag(const std::string& name)
    {
        m_tags.erase(name);
    }

    bool HasUserString(const std::string& name)
    {
        return (m_userStrings.find(name) != m_userStrings.end());
    }

    std::string UserString(const std::string& name)
    {
        return m_userStrings.at(name);
    }

    void SetUserString(const std::string& name, const std::string& value)
    {
        m_userStrings.insert({name, value});
    }

    void RemoveUserString(const std::string& name)
    {
        m_userStrings.erase(name);
    }

public:     // python

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Attributes;

        py::class_<Type, Pointer<Type>>(m, "Attributes")
            .def("HasTag", &Type::HasTag, "name"_a)
            .def("AddTag", &Type::AddTag, "name"_a)
            .def("RemoveTag", &Type::RemoveTag, "name"_a)
            .def("HasUserString", &Type::HasUserString, "name"_a)
            .def("UserString", &Type::UserString, "name"_a)
            .def("SetUserString", &Type::SetUserString, "name"_a, "value"_a)
            .def("RemoveUserString", &Type::RemoveUserString, "name"_a)
        ;
    }
};

} // namespace ANurbs