#pragma once

#include "../Define.h"

#include "Attributes.h"

#include <memory>
#include <string>
#include <stdexcept>

namespace ANurbs {

class Model;

class EntryBase
{
public:
    virtual std::string key() const = 0;

    virtual std::string type_name() const = 0;

    virtual bool is_empty() const = 0;

    virtual ~EntryBase() { }
};

template <typename TData>
class Entry : public EntryBase
{
    using Attributes = typename TData::Attributes;

    std::string m_key;
    std::shared_ptr<TData> m_data;
    std::shared_ptr<Attributes> m_attributes;

public:
    Entry(std::shared_ptr<TData> data, std::shared_ptr<Attributes> attributes)
        : m_data(data), m_attributes(attributes)
    {
    }

    Entry(const std::string& key, std::shared_ptr<TData> data,
        std::shared_ptr<Attributes> attributes)
        : m_key(key), m_data(data), m_attributes(attributes)
    {
    }

    std::shared_ptr<TData> data()
    {
        return m_data;
    }

    void set_data(std::shared_ptr<TData> value)
    {
        m_data = value;
    }

    std::shared_ptr<Attributes> attributes() const
    {
        return m_attributes;
    }

    std::string key() const override
    {
        return m_key;
    }

    std::string type_name() const override
    {
        return TData::type_name();
    }

    static Unique<Entry<TData>> create(std::string key,
        std::shared_ptr<TData> data)
    {
        Pointer<Attributes> attributes = new_<Attributes>();
        return new_<Entry<TData>>(key, data, attributes);
    }

    static Unique<Entry<TData>> create(std::shared_ptr<TData> data)
    {
        Pointer<Attributes> attributes = new_<Attributes>();
        return new_<Entry<TData>>(data, attributes);
    }

    bool is_empty() const override
    {
        return m_data == nullptr;
    }
};

} // namespace Data