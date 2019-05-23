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
#include <stdexcept>
#include <string>
#include <vector>

namespace ANurbs {

class Model
{
    std::vector<Pointer<EntryBase>> m_entries;
    std::vector<size_t> m_entryMap;
    std::unordered_map<std::string, std::pair<size_t, size_t>> m_keyMap;

public:
    template <typename TData>
    static void Register(bool noException = false)
    {
        TypeRegistry<Model>::Register<TData>(noException);
    }

    template <typename TData>
    Ref<TData> Add(Pointer<TData> data)
    {
        if (data == nullptr) {
            throw std::invalid_argument("Data is null");
        }

        Pointer<Entry<TData>> entry = Entry<TData>::Create(data);

        m_entries.push_back(std::static_pointer_cast<EntryBase>(entry));

        m_entryMap.push_back(m_entries.size() - 1);

        return Ref<TData>(entry);
    }

    template <typename TData>
    Ref<TData> Add(const std::string& key, Pointer<TData> data)
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }
        if (data == nullptr) {
            throw std::invalid_argument("Data is null");
        }

        Pointer<Entry<TData>> entry;

        const auto it = m_keyMap.find(key);

        if (it != m_keyMap.end()) {
            const auto index = std::get<0>(it->second);

            entry = std::static_pointer_cast<Entry<TData>>(m_entries[index]);

            if (!entry->IsEmpty()) {
                throw std::runtime_error("Entry already exists");
            }

            entry->SetData(data);

            m_entryMap.push_back(index);

            it->second = { index, m_entryMap.size() - 1 };
        } else {
            const auto index = m_entries.size();

            entry = Entry<TData>::Create(key, data);

            m_entries.push_back(std::static_pointer_cast<EntryBase>(entry));

            m_entryMap.push_back(index);

            m_keyMap[key] = { index, m_entryMap.size() - 1 };
        }

        return Ref<TData>(entry);
    }

    template <typename TData>
    Ref<TData> Replace(size_t index, Pointer<TData> data)
    {
        if (data == nullptr) {
            throw std::invalid_argument("Data is null");
        }

        auto entry = std::static_pointer_cast<Entry<TData>>(
            m_entries.at(index));
        
        entry->SetData(data);

        return Ref<TData>(entry);
    }

    template <typename TData>
    Ref<TData> Replace(const std::string& key, Pointer<TData> data)
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }
        if (data == nullptr) {
            throw std::invalid_argument("Data is null");
        }

        const size_t index = std::get<0>(m_keyMap.at(key));

        return Replace(index, data);
    }

    void remove(const size_t index)
    {
        m_entryMap.erase(m_entryMap.begin() + index);
    }

    void remove(const std::string& key)
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }

        auto it = m_keyMap.find(key);

        if (it != m_keyMap.end()) {
            const size_t index = std::get<1>(it->second);
            
            remove(index);

            m_keyMap.erase(it);
        }
    }

    std::string get_type(size_t index) const
    {
        return m_entries.at(m_entryMap.at(index))->type_name();
    }

    std::string get_type(const std::string& key) const
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }

        const auto it = m_keyMap.find(key);

        if (it == m_keyMap.end()) {
            throw std::out_of_range("Entry does not exists");
        }

        const auto index = std::get<0>(it->second);

        return m_entries.at(index)->type_name();
    }

    std::string get_key(size_t index) const
    {
        return m_entries.at(m_entryMap.at(index))->Key();
    }

    template <typename TData>
    Ref<TData> Get(size_t index) const
    {
        const auto entry = std::static_pointer_cast<Entry<TData>>(
            m_entries.at(m_entryMap.at(index)));

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
            list.push_back(Get<TData>(i));
        }

        return list;
    }

    size_t
    nb_entries() const
    {
        return m_entryMap.size();
    }

    template <typename TData>
    Ref<TData> Get(std::string key) const
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }

        const auto it = m_keyMap.find(key);

        if (it == m_keyMap.end()) {
            throw std::out_of_range("Entry does not exists");
        }

        const auto index = std::get<0>(it->second);

        return std::static_pointer_cast<Entry<TData>>(
            m_entries.at(index));
    }

    template <typename TData>
    Ref<TData> GetLazy(std::string key)
    {
        Pointer<Entry<TData>> entry;

        const auto it = m_keyMap.find(key);

        if (it == m_keyMap.end()) {
            const auto index = m_entries.size();
            entry = Entry<TData>::Create(key, nullptr);
            m_entries.push_back(std::static_pointer_cast<EntryBase>(entry));
            m_keyMap[key] = { index, 0 };
        } else {
            entry = std::static_pointer_cast<Entry<TData>>(
                m_entries[std::get<0>(it->second)]);
        }

        return Ref<TData>(entry);
    }

    bool Contains(const std::string& key)
    {
        return m_keyMap.find(key) != m_keyMap.end();
    }

    void load(const std::string& path)
    {
        JsonReader<Model>::load_file(*this, path);
    }

    void save(const std::string& path)
    {
        JsonWriter<Model>::save_file(*this, path);
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
            .def("load", &Type::load, "path"_a)
            .def("save", &Type::save, "path"_a)
            .def("add_array", &Type::add_array, "content"_a)
            .def("add_object", &Type::add_object, "content"_a)
            .def("nb_entries", &Type::nb_entries)
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
        ;
    }

    template <typename TData>
    static void register_python_data_type(pybind11::module& m,
        pybind11::class_<Model, Pointer<Model>>& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        Model::Register<TData>();

        Ref<TData>::register_python(m);

        PythonDataTypeBase<Model>::s_types[TData::type_name()] =
            new_<PythonDataType<Model, TData>>();

        model.def("add", &PythonDataType<Model, TData>::add, "data"_a);

        model.def("add", &PythonDataType<Model, TData>::add_with_key, "key"_a,
            "data"_a);

        model.def("add", [](ANurbs::Model& self, const Ref<TData>& ref) {
            return PythonDataType<Model, TData>::add_with_key(self, ref.Key(),
            ref.Data()); }, "ref"_a);
        
        model.def("replace", &PythonDataType<Model, TData>::replace, "index"_a,
            "data"_a);

        model.def("replace", &PythonDataType<Model, TData>::replace_with_key,
            "key"_a, "data"_a);
    };
};

} // namespace ANurbs