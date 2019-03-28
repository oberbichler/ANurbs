#pragma once

#include "Define.h"
#include "Attributes.h"

#include <ANurbs/src/Pointer.h>

#include <memory>
#include <string>
#include <stdexcept>

namespace ANurbs {

class Model;

class EntryBase
{
public:
    virtual std::string Key() const = 0;

    virtual std::string Type() const = 0;

    virtual bool IsEmpty() const = 0;

    virtual ~EntryBase() { }
};

template <typename TData>
class Entry : public EntryBase
{
    using AttributesType = AttributeTypeOf<TData>;

    std::string m_key;
    std::shared_ptr<TData> m_data;
    std::shared_ptr<AttributesType> m_attributes;

public:
    Entry(
        std::shared_ptr<TData> data,
        std::shared_ptr<AttributesType> attributes)
    : m_data(data)
    , m_attributes(attributes)
    {
    }

    Entry(
        const std::string& key,
        std::shared_ptr<TData> data,
        std::shared_ptr<AttributesType> attributes)
    : m_key(key)
    , m_data(data)
    , m_attributes(attributes)
    {
    }

    std::shared_ptr<TData>
    Data()
    {
        return m_data;
    }

    void
    SetData(
        std::shared_ptr<TData> value)
    {
        m_data = value;
    }

    std::shared_ptr<AttributesType>
    Attributes() const
    {
        return m_attributes;
    }

    std::string
    Key() const override
    {
        return m_key;
    }

    std::string
    Type() const override
    {
        return TypeStringOf<TData>();
    }

    static Unique<Entry<TData>>
    Create(
        std::string key,
        std::shared_ptr<TData> data)
    {
        Pointer<AttributesType> attributes = New<AttributesType>();
        return New<Entry<TData>>(key, data, attributes);
    }

    static Unique<Entry<TData>>
    Create(
        std::shared_ptr<TData> data)
    {
        Pointer<AttributesType> attributes = New<AttributesType>();
        return New<Entry<TData>>(data, attributes);
    }

    bool
    IsEmpty() const override
    {
        return m_data == nullptr;
    }
};

} // namespace Data