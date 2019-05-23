#pragma once

#include "../Define.h"

#include "Entry.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace ANurbs {

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

    std::string Key() const
    {
        return m_entry->Key();
    }

    std::string type_name() const
    {
        return m_entry->type_name();
    }

    Pointer<TData> Data() const
    {
        return m_entry->Data();
    }

    // Pointer<typename Entry<TData>::AttributeType> Attributes() const
    // {
    //     return m_entry->Attributes();
    // }

    TData& operator*()
    {
        return *Data();
    }

    const TData& operator*() const
    {
        return *Data();
    }

    Pointer<TData> operator->() const
    {
        return Data();
    }

    bool IsEmpty() const
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
            .def_property_readonly("key", &Ref<TData>::Key)
            .def_property_readonly("type", &Ref<TData>::type_name)
            .def_property_readonly("data", &Ref<TData>::Data)
            // .def("Attributes", &Ref<TData>::Attributes)
        ;
    }
};

} // namespace ANurbs