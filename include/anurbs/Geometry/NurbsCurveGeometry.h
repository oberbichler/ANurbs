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
    
    std::vector<Vector> derivatives_at(const double t, const Index order)
        const override
    {
        NurbsCurveShapeFunction shape_function;

        shape_function.resize(m_degree, order);

        if (m_weights.size() > 0) {
            shape_function.compute(m_knots, [&](Index i) { return weight(i); }, t);
        } else {
            shape_function.compute(m_knots, t);
        }

        std::vector<Vector> derivatives(shape_function.nb_shapes());

        for (Index order = 0; order < shape_function.nb_shapes(); order++) {
            for (Index i = 0; i < shape_function.nb_nonzero_poles(); i++) {
                Index index = shape_function.first_nonzero_pole() + i;

                if (i == 0) {
                    derivatives[order] = pole(index) * shape_function(order, i);
                } else {
                    derivatives[order] += pole(index) * shape_function(order, i);
                }
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

    double& knot(Index i)
    {
        return m_knots[i];
    }

    double knot(Index i) const
    {
        return m_knots[i];
    }

    Eigen::Ref<Eigen::VectorXd> knots()
    {
        return m_knots;
    }

    Eigen::Ref<const Eigen::VectorXd> knots() const
    {
        return m_knots;
    }

    Index nb_knots() const
    {
        return static_cast<Index>(m_knots.size());
    }

    Index nb_poles() const
    {
        return static_cast<Index>(m_poles.rows());
    }

    Vector point_at(const double t) const override
    {
        NurbsCurveShapeFunction shape_function;

        shape_function.resize(m_degree, 0);

        if (m_weights.size() > 0) {
            shape_function.compute(m_knots, [&](Index i) { return weight(i); }, t);
        } else {
            shape_function.compute(m_knots, t);
        }

        Vector point;

        for (Index i = 0; i < shape_function.nb_nonzero_poles(); i++) {
            const Index index = shape_function.first_nonzero_pole() + i;

            if (i == 0) {
                point = pole(index) * shape_function(0, i);
            } else {
                point += pole(index) * shape_function(0, i);
            }
        }

        return point;
    }

    Eigen::Ref<Vector> pole(Index i)
    {
        return m_poles.row(i);
    }

    Eigen::Ref<const Vector> pole(Index i) const
    {
        return m_poles.row(i);
    }

    Eigen::Ref<Poles> poles()
    {
        return m_poles;
    }

    Eigen::Ref<const Poles> poles() const
    {
        return m_poles;
    }

    void set_poles(Eigen::Ref<const Poles> poles)
    {
        m_poles = poles;
    }
    
    void set_knot(Index i, double value)
    {
        m_knots[i] = value;
    }
    
    void set_pole(Index i, Vector value)
    {
        m_poles.row(i) = value;
    }
    
    void set_weight(Index i, double value)
    {
        m_weights(i) = value;
    }

    std::pair<std::vector<Index>, linear_algebra::MatrixXd> shape_functions_at(const double t,
        const Index order) const
    {
        NurbsCurveShapeFunction shape_function(degree(), order);

        if (is_rational()) {
            shape_function.compute(knots(), [&](Index i) {
                return weight(i); }, t);
        } else {
            shape_function.compute(knots(), t);
        }

        linear_algebra::MatrixXd values(shape_function.nb_shapes(),
            shape_function.nb_nonzero_poles());

        for (Index i = 0; i < shape_function.nb_shapes(); i++) {
            for (Index j = 0; j < shape_function.nb_nonzero_poles(); j++) {
                values(i, j) = shape_function(i, j);
            }
        }

        return {shape_function.nonzero_pole_indices(), values};
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
    
    double weight(Index i) const
    {
        return m_weights(i);
    }

    Eigen::Ref<Eigen::VectorXd> weights()
    {
        return m_weights;
    }

    Eigen::Ref<const Eigen::VectorXd> weights() const
    {
        return m_weights;
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
        const auto is_rational = reader.has("weigths");

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
            .def(py::init<const Index, const Index, const bool>(), "degree"_a,
                "nb_poles"_a, "is_rational"_a)
            .def(py::init<const Index, const Eigen::VectorXd,
                const Poles>(), "degree"_a, "knots"_a, "poles"_a)
            .def(py::init<const Index, const Eigen::VectorXd,
                const Poles, const Eigen::VectorXd>(),
                "degree"_a, "knots"_a, "poles"_a, "weights"_a)
            .def(py::init<const Index, const Eigen::VectorXd,
                const std::vector<ControlPoint>>(), "degree"_a, "knots"_a, "control_points"_a)
            // read-only properties
            .def_property_readonly("is_rational", &Type::is_rational)
            .def_property_readonly("knots", py::overload_cast<>(&Type::knots))
            .def_property_readonly("nb_knots", &Type::nb_knots)
            .def_property_readonly("nb_poles", &Type::nb_poles)
            .def_property_readonly("poles", py::overload_cast<>(&Type::poles))
            .def_property_readonly("weights", py::overload_cast<>(&Type::weights))
            // methods
            .def("knot", py::overload_cast<Index>(&Type::knot, py::const_), "index"_a)
            .def("set_knot", &Type::set_knot, "index"_a, "value"_a)
            .def("set_pole", &Type::set_pole, "index"_a, "value"_a)
            .def("set_weight", &Type::set_weight, "index"_a, "value"_a)
            .def("shape_functions_at", &Type::shape_functions_at, "t"_a,
                "order"_a)
            .def("weight", &Type::weight, "index"_a)
            .def("greville_point", &Type::greville_point, "index"_a)
            // .def("clone", &Type::clone)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs