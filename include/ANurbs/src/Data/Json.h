#pragma once

#include "Ref.h"

#include <nlohmann/fifo_map.hpp>
#include <nlohmann/json.hpp>

#include <ANurbs/Core>

#include <stdexcept>

namespace ANurbs {

template <typename TData>
void
to_json(
    nlohmann::json& json,
    const Ref<TData>& ref)
{
    if (ref.Key().empty()) {
        throw std::runtime_error("Entity has no key");
    }
    
    json = ref.Key();
}

template <typename TData>
void
to_json(
    nlohmann::json& json,
    const Grid<TData>& grid)
{
    for (int i = 0; i < grid.NbValues(); i++) {
        json.push_back(grid[i]);
    }
}

template <int TDimension>
void
to_json(
    nlohmann::json& json,
    const Point<double, TDimension>& point)
{
    for (int i = 0; i < TDimension; i++) {
        json.push_back(point[i]);
    }
}

template <int TDimension>
void
from_json(
    const nlohmann::json& json,
    Point<double, TDimension>& point)
{
    assert(json.size() == TDimension);

    for (int i = 0; i < TDimension; i++) {
        json.at(i).get_to(point[i]);
    }
}

template<class TKey, class TValue, class TCompare, class TAllocator>
using FiFoMap = nlohmann::fifo_map<TKey, TValue,
    nlohmann::fifo_map_compare<TKey>, TAllocator>;

using Json = nlohmann::basic_json<FiFoMap>;

using ToJson = nlohmann::json;

static std::string
GetKeyFromJson(
    const Json& source)
{
    const auto it = source.find("Key");

    if (it == source.end()) {
        return std::string();
    }

    return it.value();
}

static std::string
GetTypeFromJson(
    const Json& source)
{
    const auto it = source.find("Type");

    if (it == source.end()) {
        throw std::runtime_error("Type is missing");
    }

    return it.value();
}

} // namespace ANurbs