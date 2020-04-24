#pragma once

#include "../Define.h"

#include "Entry.h"
#include "JsonReader.h"
#include "JsonWriter.h"
#include "Ref.h"
#include "PythonDataType.h"

#include <iostream>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace anurbs {

class Model
{
private:    // variables
    std::vector<Pointer<EntryBase>> m_entries;
    std::vector<size_t> m_entry_map;
    std::unordered_map<std::string, std::pair<size_t, size_t>> m_key_map;

public:     // static methods
    static Pointer<Model> from_file(const std::string& path)
    {
        auto model = new_<Model>();

        model->load(path);

        return model;
    }

public:     // methods
    template <typename TData>
    static void register_type(bool no_exception = false)
    {
        TypeRegistry<Model>::register_type<TData>(no_exception);
    }

    template <typename TData>
    Ref<TData> add(Pointer<TData> data)
    {
        if (data == nullptr) {
            throw std::invalid_argument("Data is null");
        }

        Pointer<Entry<TData>> entry = Entry<TData>::create(data);

        m_entries.push_back(std::static_pointer_cast<EntryBase>(entry));

        m_entry_map.push_back(m_entries.size() - 1);

        return Ref<TData>(entry);
    }

    template <typename TData>
    Ref<TData> add(Pointer<TData> data, const std::string& attributes)
    {
        const auto ref = add(data);

        ref.attributes()->load(*this, Json::parse(attributes));

        return ref;
    }

    template <typename TData>
    Ref<TData> add(const std::string& key, Pointer<TData> data)
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }
        if (data == nullptr) {
            throw std::invalid_argument("Data is null");
        }

        Pointer<Entry<TData>> entry;

        const auto it = m_key_map.find(key);

        if (it != m_key_map.end()) {
            const auto index = std::get<0>(it->second);

            entry = std::static_pointer_cast<Entry<TData>>(m_entries[index]);

            if (!entry->is_empty()) {
                throw std::runtime_error("Entry already exists");
            }

            entry->set_data(data);

            m_entry_map.push_back(index);

            it->second = { index, m_entry_map.size() - 1 };
        } else {
            const auto index = m_entries.size();

            entry = Entry<TData>::create(key, data);

            m_entries.push_back(std::static_pointer_cast<EntryBase>(entry));

            m_entry_map.push_back(index);

            m_key_map[key] = { index, m_entry_map.size() - 1 };
        }

        return Ref<TData>(entry);
    }

    template <typename TData>
    Ref<TData> replace(size_t index, Pointer<TData> data)
    {
        if (data == nullptr) {
            throw std::invalid_argument("Data is null");
        }

        auto entry = std::static_pointer_cast<Entry<TData>>(
            m_entries.at(index));
        
        entry->set_data(data);

        return Ref<TData>(entry);
    }

    template <typename TData>
    Ref<TData> replace(const std::string& key, Pointer<TData> data)
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }
        if (data == nullptr) {
            throw std::invalid_argument("Data is null");
        }

        const size_t index = std::get<0>(m_key_map.at(key));

        return replace(index, data);
    }

    void remove(const size_t index)
    {
        m_entry_map.erase(m_entry_map.begin() + index);
    }

    void remove(const std::string& key)
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }

        auto it = m_key_map.find(key);

        if (it != m_key_map.end()) {
            const size_t index = std::get<1>(it->second);
            
            remove(index);

            m_key_map.erase(it);
        }
    }

    std::string get_type(size_t index) const
    {
        return m_entries.at(m_entry_map.at(index))->type_name();
    }

    std::string get_type(const std::string& key) const
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }

        const auto it = m_key_map.find(key);

        if (it == m_key_map.end()) {
            throw std::out_of_range("Entry does not exists");
        }

        const auto index = std::get<0>(it->second);

        return m_entries.at(index)->type_name();
    }

    std::string get_key(size_t index) const
    {
        return m_entries.at(m_entry_map.at(index))->key();
    }

    template <typename TData>
    Ref<TData> get(size_t index) const
    {
        const auto entry = std::static_pointer_cast<Entry<TData>>(
            m_entries.at(m_entry_map.at(index)));

        return Ref<TData>(entry);
    }

    template <typename TData>
    std::vector<Ref<TData>> of_type() const
    {
        std::vector<Ref<TData>> list;

        for (size_t i = 0; i < nb_entries(); i++) {
            if (get_type(i) != TData::type_name()) {
                continue;
            }
            list.push_back(get<TData>(i));
        }

        return list;
    }

    size_t nb_entries() const
    {
        return m_entry_map.size();
    }

    template <typename TData>
    Ref<TData> get(std::string key) const
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }

        const auto it = m_key_map.find(key);

        if (it == m_key_map.end()) {
            throw std::out_of_range("Entry does not exists");
        }

        const auto index = std::get<0>(it->second);

        return std::static_pointer_cast<Entry<TData>>(
            m_entries.at(index));
    }

    template <typename TData>
    Ref<TData> get_lazy(std::string key)
    {
        Pointer<Entry<TData>> entry;

        const auto it = m_key_map.find(key);

        if (it == m_key_map.end()) {
            const auto index = m_entries.size();
            entry = Entry<TData>::create(key, nullptr);
            m_entries.push_back(std::static_pointer_cast<EntryBase>(entry));
            m_key_map[key] = { index, 0 };
        } else {
            entry = std::static_pointer_cast<Entry<TData>>(
                m_entries[std::get<0>(it->second)]);
        }

        return Ref<TData>(entry);
    }

    bool contains(const std::string& key)
    {
        return m_key_map.find(key) != m_key_map.end();
    }

    void load(const std::string& path)
    {
        JsonReader<Model>::load_file(*this, path);
    }

    void save(const std::string& path)
    {
        JsonWriter<Model>::save_file(*this, path);
    }

    std::string to_string()
    {
        std::stringstream string_stream;
        JsonWriter<Model>::save_stream(*this, string_stream);
        return string_stream.str();
    }

    void add_array(const std::string& content)
    {
        JsonReader<Model>::load_array(*this, content);
    }

    void add_object(const std::string& content)
    {
        JsonReader<Model>::load_object(*this, content);
    }

public:     // python
    static pybind11::class_<Model, Pointer<Model>> register_python(
        pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Model;
        using Holder = Pointer<Type>;

        return pybind11::class_<Type, Holder>(m, "Model")
            .def(py::init<>())
            // static methods
            .def_static("from_file", &Type::from_file)
            // methods
            .def("load", &Type::load, "path"_a)
            .def("save", &Type::save, "path"_a)
            .def("to_string", &Type::to_string)
            .def("add_array", &Type::add_array, "content"_a)
            .def("add_object", &Type::add_object, "content"_a)
            .def("get_type", (std::string (Type::*)(const size_t) const)
                &Type::get_type, "index"_a)
            .def("get_type", (std::string (Type::*)(const std::string&) const)
                &Type::get_type, "key"_a)
            .def("get_key", &Type::get_key, "index"_a)
            .def("get", &PythonDataTypeBase<Model>::by_key, "key"_a)
            .def("get", &PythonDataTypeBase<Model>::by_index, "index"_a)
            .def("of_type", &PythonDataTypeBase<Model>::by_type, "type"_a)
            .def("remove", (void (Type::*)(const size_t))
                &Type::remove, "index"_a)
            .def("remove", (void (Type::*)(const std::string&))
                &Type::remove, "key"_a)
            // read-only properties
            .def_property_readonly("nb_entries", &Type::nb_entries)
        ;
    }

    template <typename TData>
    static void register_python_data_type(pybind11::module& m,
        pybind11::class_<Model, Pointer<Model>>& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        Model::register_type<TData>();

        Ref<TData>::register_python(m);

        PythonDataTypeBase<Model>::s_types[TData::type_name()] =
            new_<PythonDataType<Model, TData>>();

        model.def("add", &PythonDataType<Model, TData>::add, "data"_a);

        model.def("add", &PythonDataType<Model, TData>::add_with_attributes,
            "data"_a, "attributes"_a);

        model.def("add", &PythonDataType<Model, TData>::add_with_key, "key"_a,
            "data"_a);

        model.def("add", [](anurbs::Model& self, const Ref<TData>& ref) {
            return PythonDataType<Model, TData>::add_with_key(self, ref.key(),
            ref.data()); }, "ref"_a);
        
        model.def("replace", &PythonDataType<Model, TData>::replace, "index"_a,
            "data"_a);

        model.def("replace", &PythonDataType<Model, TData>::replace_with_key,
            "key"_a, "data"_a);
    };
};

} // namespace anurbs