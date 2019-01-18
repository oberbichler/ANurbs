#pragma once

#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/Core>

#include <vector>

namespace ANurbs {

template <typename TVector>
class Line
{
private:
    TVector m_start;
    TVector m_end;

public:
    using DataType = Line<TVector>;

    static std::string
    Type()
    {
        return "Line" + std::to_string(DimensionOf<TVector>()) + "D";
    }

    TVector
    Start() const
    {
        return m_start;
    }

    void
    SetStart(
        const TVector& value)
    {
        m_start = value;
    }

    TVector
    End() const
    {
        return m_end;
    }

    void
    SetEnd(
        const TVector& value)
    {
        m_end = value;
    }

    static Unique<Line>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<Line>();

        result->m_start = data.at("Start");
        result->m_end = data.at("End");

        return result;
    }

    void
    Save(
        const Model& model,
        Json& data) const
    {
        data["Start"] = ToJson(m_start);
        data["End"] = ToJson(m_end);
    }
};

template <typename TVector>
struct AttributesType<Line<TVector>>
{
    using Type = CadAttributes;
};

} // namespace ANurbs