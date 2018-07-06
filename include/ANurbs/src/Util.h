#pragma once

#include <functional>


namespace ANurbs {
namespace Util {

template <typename TContainer>
struct Weights;

template <typename TWeight>
struct Weights<std::vector<TWeight>>
{
    using Type = TWeight;
    using ContainerType = std::vector<TWeight>;

    static Type
    Get(
        const ContainerType& container,
        const std::size_t& index
    )
    {
        return container[index];
    }
};

template <typename TContainer>
struct Weights
{
    using Type = typename std::result_of_t<TContainer(std::size_t)>;
    using ContainerType = TContainer;

    static Type
    Get(
        const ContainerType& container,
        const std::size_t& index
    )
    {
        return container(index);
    }
};

} // Util
} // ANurbs
