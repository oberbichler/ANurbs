#pragma once

#include "../../Model/CadAttributes.h"
#include "../../Model/Json.h"
#include "../../Model/Model.h"
#include "../../Model/Ref.h"

#include <ANurbs/ANurbs.h>

#include <vector>

namespace ANurbs {

template <int TDimension>
struct AttributesType<Surface<TDimension, Ref<NurbsSurfaceGeometry<TDimension>>>>
{
    using Type = CadAttributes;
};

template <int TDimension>
struct DataIO<Surface<TDimension, Ref<NurbsSurfaceGeometry<TDimension>>>>
{
    using DataType = Surface<TDimension, Ref<NurbsSurfaceGeometry<TDimension>>>;

    static std::string
    Type()
    {
        return "Surface" + std::to_string(TDimension) + "D";
    }

    static Unique<DataType> Load(Model& model, const Json& source)
    {
        const auto geometry = model.GetLazy<NurbsSurfaceGeometry<TDimension>>(source.at("Geometry"));

        auto result = New<DataType>(geometry);

        return result;
    }

    static void
    Save(
        const Model& model,
        const DataType& data,
        Json& target)
    {
        target["Geometry"] = data.surface_geometry().Key();
    }
};

} // namespace ANurbs