#include <ANurbs/Python>
#include <ANurbs/EigenSupport>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>


PYBIND11_MODULE(ANurbs, m) {
    m.doc() = "A simple NURBS library for Python";

    using TypeFactory = ANurbs::EigenTypeFactory;

    RegisterAlgorithms<TypeFactory>(m);
    RegisterData<TypeFactory>(m);
}