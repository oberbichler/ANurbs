#pragma once

#include "Define.h"

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

    std::string Type() const
    {
        return m_entry->Type();
    }

    Pointer<TData> Data() const
    {
        return m_entry->Data();
    }

    Pointer<AttributeTypeOf<TData>> Attributes() const
    {
        return m_entry->Attributes();
    }

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
    template <typename TModule>
    static void register_python(TModule& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        const std::string name = std::string("Ref") + TData::type_name();

        py::class_<Ref<TData>>(m, name.c_str())
            .def("Key", &Ref<TData>::Key)
            .def("Type", &Ref<TData>::Type)
            .def("Data", &Ref<TData>::Data)
            .def("Attributes", &Ref<TData>::Attributes)
        ;
    }
};

} // namespace ANurbs