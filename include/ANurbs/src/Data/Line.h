#pragma once

#include "../../Model/CadAttributes.h"
#include "../../Model/Json.h"
#include "../../Model/Model.h"
#include "../../Model/Ref.h"

#include <ANurbs/ANurbs.h>

#include <vector>

namespace ANurbs {

template <int TDimension>
class Line
{
public:
    using Vector = Vector<TDimension>;

private:
    Vector m_a;
    Vector m_b;

public:
    using DataType = Line<TDimension>;

    Line()
    { }
    
    Line(
        const Vector& a,
        const Vector& b)
    : m_a(a)
    , m_b(b)
    { }

    static std::string
    Type()
    {
        return "Line" + std::to_string(TDimension) + "D";
    }

    Vector
    A() const
    {
        return m_a;
    }

    void
    SetA(
        const Vector& value)
    {
        m_a = value;
    }

    Vector
    B() const
    {
        return m_b;
    }

    void
    SetB(
        const Vector& value)
    {
        m_b = value;
    }

    static Unique<Line>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<Line>();

        // result->m_a = data.at("A");
        // result->m_b = data.at("B");

        return result;
    }

    void
    Save(
        const Model& model,
        Json& data) const
    {
        // data["A"] = ToJson(m_a);
        // data["B"] = ToJson(m_b);
    }
};

template <int TDimension>
struct AttributesType<Line<TDimension>>
{
    using Type = CadAttributes;
};

} // namespace ANurbs