#pragma once

#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/ANurbs.h>

#include <vector>

namespace ANurbs {

template <int TDimension>
struct AttributesType<Curve<TDimension, Ref<NurbsCurveGeometry<TDimension>>>>
{
    using Type = CadAttributes;
};

template <int TDimension>
struct DataIO<Curve<TDimension, Ref<NurbsCurveGeometry<TDimension>>>>
{
    using DataType = Curve<TDimension, Ref<NurbsCurveGeometry<TDimension>>>;

    static std::string
    Type()
    {
        return "Curve" + std::to_string(DataType::dimension()) + "D";
    }

    static Unique<DataType>
    Load(
        Model& model,
        const Json& source)
    {
        const auto geometry = model.GetLazy<NurbsCurveGeometry<TDimension>>(
            source.at("Geometry"));

        const Interval domain = source.at("Domain");

        return New<DataType>(geometry, domain);
    }

    static void
    Save(
        const Model& model,
        const DataType& data,
        Json& target)
    {
        target["Geometry"] = data.curve_geometry().Key();
        target["Domain"] = ToJson(data.domain());
    }
};

} // namespace ANurbs