#pragma once

#include "Define.h"
#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/Core>

#include <Eigen/Core>

#include <string>
#include <vector>


namespace ANurbs {

template <typename TVector>
class TextDot
{
private:
    TVector m_location;
    std::string m_text;

public:
    using DataType = TextDot<TVector>;

    static std::string
    Type()
    {
        return "TextDot" + std::to_string(DimensionOf<TVector>()) + "D";
    }

    TextDot()
    {
    }

    TextDot(
        const TVector& location,
        const std::string& text)
    : m_location(location)
    , m_text(text)
    {
    }

    TVector
    Location() const
    {
        return m_location;
    }

    void
    SetLocation(
        const TVector& value)
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
        auto data = New<TextDot<TVector>>();

        data->m_location = source.at("Location");
        data->m_text = source.at("Text");

        return data;
    }

    void
    Save(
        const Model& model,
        Json& target) const
    {
        target["Location"] = ToJson(m_location);
        target["Text"] = ToJson(m_text);
    }
};

template <typename TVector>
struct AttributesType<TextDot<TVector>>
{
    using Type = CadAttributes;
};

} // namespace ANurbs