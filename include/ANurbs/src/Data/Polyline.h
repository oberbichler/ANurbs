#pragma once

#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/Core>

#include <vector>

namespace ANurbs {

template <typename TVector>
class Polyline
{
private:
    std::vector<TVector> m_points;

public:
    using DataType = Polyline<TVector>;

    static std::string
    Type()
    {
        return "Polyline" + std::to_string(DimensionOf<TVector>()) + "D";
    }

    size_t
    NbPoints() const
    {
        return m_points.size();
    }

    TVector
    Point(
        const size_t index) const
    {
        return m_points[index];
    }

    void
    SetPoint(
        const size_t index,
        const TVector& value)
    {
        m_points[index] = value;
    }

    static Unique<Polyline<TVector>>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<Polyline<TVector>>();

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

template <typename TVector>
struct AttributesType<Polyline<TVector>>
{
    using Type = CadAttributes;
};

} // namespace ANurbs