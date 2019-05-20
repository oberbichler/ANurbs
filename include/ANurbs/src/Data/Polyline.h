#pragma once

#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/ANurbs.h>

#include <vector>

namespace ANurbs {

template <int TDimension>
class Polyline
{
public:
    using Vector = Vector<TDimension>;

private:
    std::vector<Vector> m_points;

public:
    using DataType = Polyline<TDimension>;

    Polyline()
    { }
    
    Polyline(
        const int nbPoints)
    : m_points(nbPoints)
    { }

    static std::string
    Type()
    {
        return "Polyline" + std::to_string(TDimension) + "D";
    }

    size_t
    NbPoints() const
    {
        return m_points.size();
    }

    Vector
    Point(
        const size_t index) const
    {
        return m_points[index];
    }

    void
    SetPoint(
        const size_t index,
        const Vector& value)
    {
        m_points[index] = value;
    }

    static Unique<Polyline<TDimension>>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<Polyline<TDimension>>();

        // Load Points
        {
            const auto points = data.at("Points");

            result->m_points.resize(points.size());

            for (size_t i = 0; i < points.size(); i++) {
                result->m_points[i] = points[i];
            }
        }

        return result;
    }

    void
    Save(
        const Model& model,
        Json& data) const
    {
        data["Points"] = ToJson(m_points);
    }
};

template <int TDimension>
struct AttributesType<Polyline<TDimension>>
{
    using Type = CadAttributes;
};

} // namespace ANurbs