#pragma once

#include "Define.h"
#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/Core>

#include <vector>

#include <Eigen/Core>

namespace Eigen {

// template <int TDimension>
// void
// to_json(
//     ANurbs::Json& json,
//     const Eigen::Matrix<double, TDimension, 1>& point)
// {
//     for (int i = 0; i < TDimension; i++) {
//         json.push_back(point[i]);
//     }
// }

// template <int TDimension>
// Eigen::Matrix<double, TDimension, 1>
// from_json(
//     const ANurbs::Json& json)
// {
//     Eigen::Matrix<double, TDimension, 1> point;

//     assert(json.size() == TDimension);

//     for (int i = 0; i < TDimension; i++) {
//         json.at(i).get_to(point[i]);
//     }

//     return point;
// }

}

namespace ANurbs {

template <typename TVector>
class PointData
{
private:
    TVector m_location;

public:
    using DataType = PointData<TVector>;

    static std::string
    Type()
    {
        return "Point" + std::to_string(DimensionOf<TVector>()) + "D";
    }

    PointData()
    {
    }

    PointData(
        const TVector& value)
    : m_location(value)
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

    static Unique<DataType>
    Load(
        Model& model,
        const Json& source)
    {
        auto data = New<PointData<TVector>>();

        data->m_location = source.at("Location");

        return data;
    }

    void
    Save(
        const Model& model,
        Json& target) const
    {
        target["Location"] = ToJson(m_location);
    }
};

template <typename TVector>
struct AttributesType<PointData<TVector>>
{
    using Type = CadAttributes;
};

} // namespace ANurbs