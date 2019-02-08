#include <ANurbs/Python>
#include <ANurbs/EigenSupport>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>


PYBIND11_MODULE(ANurbs, m) {
    m.doc() = "A simple NURBS library for Python";
    m.attr("__author__") = "Thomas Oberbichler";
    m.attr("__copyright__") = "Copyright (c) 2018, Thomas Oberbichler";
    m.attr("__version__") = ANURBS_VERSION;
    m.attr("__email__") = "thomas.oberbichler@gmail.com";
    m.attr("__status__") = "Development";

    using TypeFactory = ANurbs::EigenTypeFactory;

    RegisterData<TypeFactory>(m);
}