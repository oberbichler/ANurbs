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

        const Interval<double> domain = source.at("Domain");

        return New<DataType>(geometry, domain);
    }

    static void
    Save(
        const Model& model,
        const DataType& data,
        Json& target)
    {
        target["Geometry"] = data.CurveGeometry().Key();
        target["Domain"] = ToJson(data.Domain());
    }
};

} // namespace ANurbs