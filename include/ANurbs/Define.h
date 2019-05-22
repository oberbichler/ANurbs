#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <nlohmann/json.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>

namespace ANurbs {

const int Dynamic = Eigen::Dynamic;

template <int TSize>
using Vector = Eigen::Matrix<double, 1, TSize>;

using VectorXd = Eigen::Matrix<double, 1, Dynamic>;

template <int TRows, int TCols>
using Matrix = Eigen::Matrix<double, TRows, TCols>;

using MatrixXd = Eigen::Matrix<double, Dynamic, Dynamic>;

template <typename T>
using Map = Eigen::Map<T>;

// template <typename T>
// using Ref = Eigen::Ref<T>;

template <typename T>
using Pointer = std::shared_ptr<T>;

template <typename T>
using Unique = std::unique_ptr<T>;

template <typename T, typename... TArgs>
Unique<T> new_(TArgs&&... args)
{
    return Unique<T>(new T(std::forward<TArgs>(args)...));
}

double dot(const double& u, const double& v)
{
    return u * v;
}

double dot(const Vector<2>& u, const Vector<2>& v)
{
    return u.dot(v);
}

double dot(const Vector<3>& u, const Vector<3>& v)
{
    return u.dot(v);
}

double squared_norm(const double& value)
{
    return std::abs(value) * std::abs(value);
}

double squared_norm(const Vector<2>& vector)
{
    return vector.squaredNorm();
}

double squared_norm(const Vector<3>& vector)
{
    return vector.squaredNorm();
}

double norm(const double& value)
{
    return std::abs(value);
}

double norm(const Vector<2>& vector)
{
    return vector.norm();
}

double norm(const Vector<3>& vector)
{
    return vector.norm();
}

double cross(const Vector<2> u, const Vector<2> v)
{
    return v(0) * u(1) - v(1) * u(0);
}

Vector<3> cross(const Vector<3> u, const Vector<3> v)
{
    return u.cross(v);
}

template <size_t I,typename T> 
struct tuple_n
{
    template <typename... Args>
    using type = typename tuple_n<I - 1, T>::template type<T, Args...>;
};

template <typename T> 
struct tuple_n<0, T> {
    template <typename... Args>
    using type = std::tuple<Args...>;   
};

template <size_t I, typename T>
using tuple_of = typename tuple_n<I, T>::template type<>;

} // namespace ANurbs