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
private:    // types
    using Type = Ref<TData>;

private:    // variables
    Pointer<Entry<TData>> m_entry;

public:     // constructors
    Ref()
    {
    }

    Ref(Pointer<Entry<TData>> entry) : m_entry(entry)
    {
    }

public:     // methods
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

        py::class_<Type>(m, name.c_str())
            // methods
            .def("__len__", [](const Type& self) { return 2; })
            .def("__getitem__", [](const Type& self, const int i) -> py::object {
                switch (i) {
                case 0:
                    return py::cast(self.key());
                case 1:
                    return py::cast(self.data());
                default:
                    throw py::index_error();
                }
            })
            // read-only properties
            .def_property_readonly("key", &Type::key)
            .def_property_readonly("type", &Type::type_name)
            .def_property_readonly("data", &Type::data)
            .def_property_readonly("attributes", &Type::attributes)
        ;
    }
};

} // namespace anurbs