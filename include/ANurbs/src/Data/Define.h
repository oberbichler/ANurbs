#pragma once

#include "Attributes.h"
#include "CadAttributes.h"

#include <ANurbs/src/Pointer.h>

namespace ANurbs {

template <typename TData>
struct DataIO
{
    static std::string
    Type()
    {
        return TData::Type();
    }

    template <typename TModel, typename TSource>
    static Unique<TData>
    Load(
        TModel& model,
        const TSource& source)
    {
        return TData::Load(model, source);
    }

    template <typename TModel, typename TTarget>
    static void
    Save(
        const TModel& model,
        const TData& data,
        TTarget& target)
    {
        data.Save(model, target);
    }
};

template <typename TData>
std::string
TypeStringOf()
{
    return DataIO<TData>::Type();
}


template <typename TData>
struct AttributesType;

template <typename TData>
using AttributeTypeOf = typename AttributesType<TData>::Type;


template <typename TData>
struct AttributesType
{
    using Type = Attributes;
};

// --- Brep

class Brep;

template <>
struct AttributesType<Brep>
{
    using Type = CadAttributes;
};

} // namespace ANurbs