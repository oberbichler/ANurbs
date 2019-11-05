#pragma once

#include "../Define.h"

#include "Entry.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace anurbs {

template <typename TData>
class Ref
{
    Pointer<Entry<TData>> m_entry;

public:
    Ref()
    {
    }

    Ref(Pointer<Entry<TData>> entry) : m_entry(entry)
    {
    }

    std::string key() const
    {
        return m_entry->key();
    }

    std::string type_name() const
    {
        return m_entry->type_name();
    }

    Pointer<TData> data() const
    {
        return m_entry->data();
    }

    Pointer<Attributes> attributes() const
    {
        return m_entry->attributes();
    }

    TData& operator*()
    {
        return *data();
    }

    const TData& operator*() const
    {
        return *data();
    }

    Pointer<TData> operator->() const
    {
        return data();
    }

    bool is_empty() const
    {
        return m_entry == nullptr;
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        const std::string name = std::string("Ref") + TData::type_name();

        py::class_<Ref<TData>>(m, name.c_str())
            .def_property_readonly("key", &Ref<TData>::key)
            .def_property_readonly("type", &Ref<TData>::type_name)
            .def_property_readonly("data", &Ref<TData>::data)
            .def_property_readonly("attributes", &Ref<TData>::attributes)
        ;
    }
};

} // namespace anurbs