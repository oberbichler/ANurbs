#pragma once

#include "Define.h"
#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/ANurbs.h>

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

template <int TDimension>
class PointData
{
public:
    using Vector = Vector<TDimension>;

private:
    Vector m_location;

public:
    using DataType = PointData<TDimension>;

    static std::string
    Type()
    {
        return "Point" + std::to_string(TDimension) + "D";
    }

    PointData()
    {
    }

    PointData(
        const Vector& value)
    : m_location(value)
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

    static Unique<DataType>
    Load(
        Model& model,
        const Json& source)
    {
        auto data = New<PointData<TDimension>>();

        // data->m_location = source.at("Location");

        return data;
    }

    void
    Save(
        const Model& model,
        Json& target) const
    {
        // target["Location"] = ToJson(m_location);
    }
};

template <int TDimension>
struct AttributesType<PointData<TDimension>>
{
    using Type = CadAttributes;
};

} // namespace ANurbs