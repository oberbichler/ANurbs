#pragma once

#include "Entry.h"
#include "JsonReader.h"
#include "JsonWriter.h"
#include "Ref.h"

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
    static void
    Register(
        bool noException = false)
    {
        TypeRegistry<Model>::Register<TData>(noException);
    }

    template <typename TData>
    Ref<TData>
    Add(
        Pointer<TData> data)
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
    Ref<TData>
    Add(
        const std::string& key,
        Pointer<TData> data)
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
    Ref<TData>
    Replace(
        size_t index,
        Pointer<TData> data)
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
    Ref<TData>
    Replace(
        const std::string& key,
        Pointer<TData> data)
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

    void
    Remove(
        const size_t index)
    {
        m_entryMap.erase(m_entryMap.begin() + index);
    }

    void
    Remove(
        const std::string& key)
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }

        auto it = m_keyMap.find(key);

        if (it != m_keyMap.end()) {
            const size_t index = std::get<1>(it->second);
            
            Remove(index);

            m_keyMap.erase(it);
        }
    }

    std::string
    GetType(
        size_t index) const
    {
        return m_entries.at(m_entryMap.at(index))->Type();
    }

    std::string
    GetType(
        const std::string& key) const
    {
        if (key.empty()) {
            throw std::invalid_argument("Key is empty");
        }

        const auto it = m_keyMap.find(key);

        if (it == m_keyMap.end()) {
            throw std::out_of_range("Entry does not exists");
        }

        const auto index = std::get<0>(it->second);

        return m_entries.at(index)->Type();
    }

    std::string
    GetKey(
        size_t index) const
    {
        return m_entries.at(m_entryMap.at(index))->Key();
    }

    template <typename TData>
    Ref<TData>
    Get(
        size_t index) const
    {
        const auto entry = std::static_pointer_cast<Entry<TData>>(
            m_entries.at(m_entryMap.at(index)));

        return Ref<TData>(entry);
    }

    template <typename TData>
    std::vector<Ref<TData>>
    GetByType() const
    {
        std::vector<Ref<TData>> list;

        for (size_t i = 0; i < NbEntries(); i++) {
            if (GetType(i) != TypeStringOf<TData>()) {
                continue;
            }
            list.push_back(Get<TData>(i));
        }

        return list;
    }

    size_t
    NbEntries() const
    {
        return m_entryMap.size();
    }

    template <typename TData>
    Ref<TData>
    Get(
        std::string key) const
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
    Ref<TData>
    GetLazy(
        std::string key)
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

    bool
    Contains(
        const std::string& key)
    {
        return m_keyMap.find(key) != m_keyMap.end();
    }

    void
    Load(
        const std::string& path)
    {
        JsonReader<Model>::LoadFile(*this, path);
    }

    void
    Save(
        const std::string& path)
    {
        JsonWriter<Model>::SaveFile(*this, path);
    }

    void
    AddArray(
        const std::string& content)
    {
        JsonReader<Model>::LoadArray(*this, content);
    }

    void
    AddObject(
        const std::string& content)
    {
        JsonReader<Model>::LoadObject(*this, content);
    }
};

} // namespace ANurbs