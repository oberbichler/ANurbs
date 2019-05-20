#pragma once

#include "Define.h"
#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/ANurbs.h>

#include <Eigen/Core>

#include <string>
#include <vector>


namespace ANurbs {

template <int TDimension>
class TextDot
{
public:
    using Vector = Vector<TDimension>;

private:
    Vector m_location;
    std::string m_text;

public:
    using DataType = TextDot<TDimension>;

    static std::string
    Type()
    {
        return "TextDot" + std::to_string(TDimension) + "D";
    }

    TextDot()
    {
    }

    TextDot(
        const Vector& location,
        const std::string& text)
    : m_location(location)
    , m_text(text)
    {
    }

    Vector
    Location() const
    {
        return m_location;
    }

    void
    SetLocation(
        const Vector& value)
    {
        m_location = value;
    }

    std::string
    Text() const
    {
        return m_text;
    }

    void
    SetText(
        const std::string& value)
    {
        m_text = value;
    }

    static Unique<DataType>
    Load(
        Model& model,
        const Json& source)
    {
        auto data = New<TextDot<TDimension>>();

        // data->m_location = source.at("Location");
        // data->m_text = source.at("Text");

        return data;
    }

    void
    Save(
        const Model& model,
        Json& target) const
    {
        // target["Location"] = ToJson(m_location);
        // target["Text"] = ToJson(m_text);
    }
};

template <int TDimension>
struct AttributesType<TextDot<TDimension>>
{
    using Type = CadAttributes;
};

} // namespace ANurbs