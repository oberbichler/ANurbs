#pragma once

#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/Core>

#include <vector>

namespace ANurbs {

template <typename TVector>
struct AttributesType<Curve<CurveGeometry<TVector>,
    Ref<CurveGeometry<TVector>>>>
{
    using Type = CadAttributes;
};

template <typename TVector>
struct DataIO<Curve<CurveGeometry<TVector>, Ref<CurveGeometry<TVector>>>>
{
    using DataType = Curve<CurveGeometry<TVector>, Ref<CurveGeometry<TVector>>>;

    static std::string
    Type()
    {
        return "Curve" + std::to_string(DimensionOf<TVector>()) + "D";
    }

    static Unique<DataType>
    Load(
        Model& model,
        const Json& source)
    {
        const auto geometry = model.GetLazy<CurveGeometry<TVector>>(
            source.at("Geometry"));

        auto result = New<DataType>(geometry, 0, 1);

        return result;
    }

    static void
    Save(
        const Model& model,
        const DataType& data,
        Json& target)
    {
    }
};

} // namespace ANurbs