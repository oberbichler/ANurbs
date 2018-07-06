#pragma once

#include <memory>

namespace ANurbs {

template <typename T>
using Pointer = std::shared_ptr<T>;

template <typename T, typename... TArgs>
Pointer<T>
Create(
    TArgs&&... args
)
{
    return std::make_shared<T>(std::forward<TArgs>(args)...);
}

} // namespace ANurbs
