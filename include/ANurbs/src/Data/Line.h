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
    TVector m_a;
    TVector m_b;

public:
    using DataType = Line<TVector>;

    Line()
    { }
    
    Line(
        const TVector& a,
        const TVector& b)
    : m_a(a)
    , m_b(b)
    { }

    static std::string
    Type()
    {
        return "Line" + std::to_string(DimensionOf<TVector>()) + "D";
    }

    TVector
    A() const
    {
        return m_a;
    }

    void
    SetA(
        const TVector& value)
    {
        m_a = value;
    }

    TVector
    B() const
    {
        return m_b;
    }

    void
    SetB(
        const TVector& value)
    {
        m_b = value;
    }

    static Unique<Line>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<Line>();

        result->m_a = data.at("A");
        result->m_b = data.at("B");

        return result;
    }

    void
    Save(
        const Model& model,
        Json& data) const
    {
        data["A"] = ToJson(m_a);
        data["B"] = ToJson(m_b);
    }
};

template <typename TVector>
struct AttributesType<Line<TVector>>
{
    using Type = CadAttributes;
};

} // namespace ANurbs