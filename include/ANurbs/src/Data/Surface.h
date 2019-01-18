#pragma once

#include "CadAttributes.h"
#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/Core>

#include <vector>

namespace ANurbs {

template <typename TVector>
struct AttributesType<Surface<SurfaceGeometry<TVector>,
    Ref<SurfaceGeometry<TVector>>>>
{
    using Type = CadAttributes;
};

template <typename TVector>
struct DataIO<Surface<SurfaceGeometry<TVector>, Ref<SurfaceGeometry<TVector>>>>
{
    using DataType = Surface<SurfaceGeometry<TVector>,
        Ref<SurfaceGeometry<TVector>>>;

    static std::string
    Type()
    {
        return "Surface" + std::to_string(DimensionOf<TVector>()) + "D";
    }

    static Unique<DataType>
    Load(
        Model& model,
        const Json& source)
    {
        const auto geometry = model.GetLazy<SurfaceGeometry<TVector>>(source.at("Geometry"));

        auto result = New<DataType>(geometry);

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