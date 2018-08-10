#pragma once

#include <memory>

namespace ANurbs {

template <typename T>
using Pointer = std::shared_ptr<T>;

template <typename T, typename... TArgs>
std::unique_ptr<T>
Create(
    TArgs&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
}

} // namespace ANurbs
