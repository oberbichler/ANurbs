#pragma once

#include "../Define.h"

#include "CurveBase.h"

#include "../Algorithm/Nurbs.h"
#include "../Algorithm/NurbsCurveShapeFunction.h"

#include "../Model/DataReader.h"
#include "../Model/DataWriter.h"
#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <stdexcept>
#include <vector>

namespace anurbs {

template <Index TDimension>
struct NurbsCurveGeometry : public CurveBase<TDimension>
{
public:     // types
    using Type = NurbsCurveGeometry<TDimension>;
    using Vector = typename CurveBase<TDimension>::Vector;
    using ControlPoint = std::pair<Vector, double>;
    using Knots = Eigen::VectorXd;
    using Poles = Eigen::Matrix<double, Eigen::Dynamic, TDimension>;
    using Weights = Eigen::VectorXd;

private:    // variables
    const Index m_degree;
    Knots m_knots;
    Poles m_poles;
    Weights m_weights;

public:     // constructors
    NurbsCurveGeometry(
        const Index degree,
        Index nb_poles,
        bool is_rational)
    : m_degree(degree)
    , m_poles(nb_poles, TDimension)
    , m_weights(is_rational ? nb_poles : 0)
    , m_knots(Nurbs::nb_knots(degree, nb_poles))
    {
        static_assert(TDimension > 0);
    }

    NurbsCurveGeometry(
        const Index degree,
        const Knots& knots,
        const Poles& poles)
    : m_degree(degree)
    , m_knots(knots)
    , m_poles(poles)
    , m_weights(0)
    {
        static_assert(TDimension > 0);

        if (length(knots) != Nurbs::nb_knots(degree, nb_poles())) {
            throw std::runtime_error("Number of knots and poles do not match");
        }
    }

    NurbsCurveGeometry(
        const Index degree,
        const Knots& knots,
        const Poles& poles,
        const Weights& weights)
    : m_degree(degree)
    , m_knots(knots)
    , m_poles(poles)
    , m_weights(weights)
    {
        static_assert(TDimension > 0);

        if (length(knots) != Nurbs::nb_knots(degree, nb_poles())) {
            throw std::runtime_error("Number of knots and poles do not match");
        }

        if (length(weights) != nb_poles()) {
            throw std::runtime_error("Number of weights and poles do not match");
        }
    }

    NurbsCurveGeometry(
        const Index degree,
        const Knots& knots,
        const std::vector<ControlPoint>& control_points)
    : m_degree(degree)
    , m_knots(knots)
    , m_poles(length(control_points), TDimension)
    , m_weights(length(control_points))
    {
        static_assert(TDimension > 0);

        if (length(knots) != Nurbs::nb_knots(degree, length(control_points))) {
            throw std::runtime_error("Number of knots and control points do not match");
        }

        for (Index i = 0; i < length(control_points); i++) {
            m_poles.row(i) = std::get<0>(control_points[i]);
            m_weights[i] = std::get<1>(control_points[i]);
        }
    }

public:     // static methods
    using CurveBase<TDimension>::dimension;

public:     // methods
    Index degree() const override
    {
        return m_degree;
    }
    
    std::vector<Vector> derivatives_at(const double t, const Index order) const override
    {
        NurbsCurveShapeFunction shape_function;

        shape_function.resize(m_degree, order);

        if (m_weights.size() > 0) {
            shape_function.compute(m_knots, m_weights, t);
        } else {
            shape_function.compute(m_knots, t);
        }

        std::vector<Vector> derivatives(shape_function.nb_shapes());

        for (Index order = 0; order < shape_function.nb_shapes(); order++) {
            derivatives[order] = Vector::Zero();

            for (Index i = 0; i < shape_function.nb_nonzero_poles(); i++) {
                Index index = shape_function.first_nonzero_pole() + i;

                derivatives[order] += pole(index) * shape_function.value(order, i);
            }
        }

        return derivatives;
    }

    Interval domain() const override
    {
        return Interval(m_knots[m_degree - 1], m_knots[nb_knots() - m_degree]);
    }

    bool is_rational() const
    {
        return m_weights.size() != 0;
    }

    // knots

    Index nb_knots() const
    {
        return static_cast<Index>(m_knots.size());
    }

    double knot(Index i) const
    {
        return m_knots[i];
    }

    double& knot(Index i)
    {
        return m_knots[i];
    }

    Eigen::Ref<const Eigen::VectorXd> knots() const
    {
        return m_knots;
    }

    Eigen::Ref<Eigen::VectorXd> knots()
    {
        return m_knots;
    }

    void set_knots(Eigen::Ref<const Eigen::VectorXd> values)
    {
        if (length(values) != nb_knots()) {
            throw std::runtime_error("Invalid size");
        }

        m_knots = values;
    }

    // poles

    Index nb_poles() const
    {
        return static_cast<Index>(m_poles.rows());
    }

    Vector pole(Index i) const
    {
        return m_poles.row(i);
    }

    Eigen::Ref<Vector> pole(Index i)
    {
        return m_poles.row(i);
    }

    Eigen::Ref<const Poles> poles() const
    {
        return m_poles;
    }

    Eigen::Ref<Poles> poles()
    {
        return m_poles;
    }

    void set_poles(Eigen::Ref<const Poles> poles)
    {
        if (poles.rows() != nb_poles() && poles.cols() != dimension()) {
            throw std::runtime_error("Invalid size");
        }

        m_poles = poles;
    }

    // weights

    double weight(Index i) const
    {
        return m_weights(i);
    }
    
    double& weight(Index i)
    {
        return m_weights(i);
    }

    Eigen::Ref<const Eigen::VectorXd> weights() const
    {
        return m_weights;
    }

    Eigen::Ref<Eigen::VectorXd> weights()
    {
        return m_weights;
    }

    void set_weights(Eigen::Ref<const Eigen::VectorXd> value)
    {
        if (!is_rational()) {
            throw std::runtime_error("Geometry is not rational");
        }

        if (length(value) != nb_poles()) {
            throw std::runtime_error("Invalid size");
        }

        m_weights = value;
    }

    //

    Vector point_at(const double t) const override
    {
        NurbsCurveShapeFunction shape_function;

        shape_function.resize(m_degree, 0);

        if (m_weights.size() > 0) {
            shape_function.compute(m_knots, m_weights, t);
        } else {
            shape_function.compute(m_knots, t);
        }

        Vector point = Vector::Zero();

        for (Index i = 0; i < shape_function.nb_nonzero_poles(); i++) {
            const Index index = shape_function.first_nonzero_pole() + i;

            point += pole(index) * shape_function.value(0, i);
        }

        return point;
    }

    std::pair<std::vector<Index>, Eigen::MatrixXd> shape_functions_at(const double t, const Index order) const
    {
        NurbsCurveShapeFunction shape_function(degree(), order);

        if (is_rational()) {
            shape_function.compute(m_knots, m_weights, t);
        } else {
            shape_function.compute(m_knots, t);
        }

        return {shape_function.nonzero_pole_indices(), shape_function.values()};
    }

    std::vector<Interval> spans() const override
    {
        const Index first_span = degree() - 1;
        const Index last_span = nb_knots() - degree() - 1;

        const Index nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (Index i = 0; i < nb_spans; i++) {
            const double t0 = knot(first_span + i);
            const double t1 = knot(first_span + i + 1);

            result[i] = Interval(t0, t1);
        }

        return result;
    }

    Index span_at(const double t) const
    {
        return Nurbs::upper_span(degree(), knots(), t);
    }

    std::vector<Index> nonzero_pole_indices_at_span(const Index span) const
    {
        std::vector<Index> result(degree() + 1);

        const Index first_nonzero_pole_index = span - degree() + 1;

        for (Index i = 0; i < length(result); i++) {
            result[i] = first_nonzero_pole_index + i;
        }

        return result;
    }

    double greville_point(Index index) const
    {
        double u = 0.0;

        for (Index i = 0; i < degree(); i++) {
            u += knot(index + i);
        }

        u /= degree();

        return u;
    }

public:     // serialization
    static std::string type_name()
    {
        return "NurbsCurveGeometry" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const DataReader reader(source);

        const auto degree = reader.read<Index>("degree");
        const auto nb_poles = reader.count("poles");
        const auto is_rational = reader.has("weights");

        auto result = new_<Type>(degree, nb_poles, is_rational);

        reader.fill_vector("knots", result->knots());
        reader.fill_matrix("poles", result->poles());

        if (is_rational) {
            reader.fill_vector("weights", result->weights());
        }

        return result;
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        DataWriter writer(target);

        writer.write("degree", data.degree());
        writer.write("nb_poles", data.nb_poles());
        writer.write_vector("knots", data.knots());
        writer.write_matrix("poles", data.poles());

        if (data.is_rational()) {
            writer.write_vector("weights", data.weights());
        }
    }

public:     // python
    static std::string python_name()
    {
        return "NurbsCurveGeometry" + std::to_string(dimension()) + "D";
    }

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = NurbsCurveGeometry<TDimension>;
        using Base = CurveBase<TDimension>; 
        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Base, Holder>(m, name.c_str())
            // constructors
            .def(py::init<const Index, const Index, const bool>(), "degree"_a, "nb_poles"_a, "is_rational"_a=false)
            .def(py::init<const Index, const Knots, const Poles>(), "degree"_a, "knots"_a, "poles"_a)
            .def(py::init<const Index, const Knots, const Poles, const Weights>(), "degree"_a, "knots"_a, "poles"_a, "weights"_a)
            .def(py::init<const Index, const Knots, const std::vector<ControlPoint>>(), "degree"_a, "knots"_a, "control_points"_a)
            // read-only properties
            .def_property_readonly("is_rational", &Type::is_rational)
            .def_property_readonly("nb_knots", &Type::nb_knots)
            .def_property_readonly("nb_poles", &Type::nb_poles)
            // properties
            .def_property("knots", py::overload_cast<>(&Type::knots), &Type::set_knots)
            .def_property("poles", py::overload_cast<>(&Type::poles), &Type::set_poles)
            .def_property("weights", py::overload_cast<>(&Type::weights), &Type::set_weights)
            // methods
            .def("greville_point", &Type::greville_point, "index"_a)
            .def("nonzero_pole_indices_at_span", &Type::nonzero_pole_indices_at_span, "span"_a)
            .def("shape_functions_at", &Type::shape_functions_at, "t"_a, "order"_a)
            .def("span_at", &Type::span_at, "t"_a)
            // .def("clone", &Type::clone)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs
