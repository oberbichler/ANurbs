#pragma once

#include "../Define.h"

#include "SurfaceBase.h"

#include "../Algorithm/Nurbs.h"
#include "../Algorithm/NurbsSurfaceShapeFunction.h"

#include "../Model/DataReader.h"
#include "../Model/DataWriter.h"
#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <stdexcept>
#include <vector>

namespace anurbs {

template <Index TDimension>
class NurbsSurfaceGeometry : public SurfaceBase<TDimension> {
public: // types
    using Type = NurbsSurfaceGeometry<TDimension>;
    using Vector = typename SurfaceBase<TDimension>::Vector;
    using Poles = Eigen::Matrix<double, Eigen::Dynamic, TDimension>;
    using Weights = Eigen::VectorXd;
    using ControlPoint = std::pair<Vector, double>;

private: // variables
    Index m_degree_u;
    Index m_degree_v;
    Index m_nb_poles_u;
    Index m_nb_poles_v;
    Eigen::VectorXd m_knots_u;
    Eigen::VectorXd m_knots_v;
    Poles m_poles;
    Eigen::VectorXd m_weights;

public: // constructor
    NurbsSurfaceGeometry(
        const Index degree_u,
        const Index degree_v,
        const Index nb_poles_u,
        const Index nb_poles_v,
        const bool is_rational)
        : m_degree_u(degree_u)
        , m_degree_v(degree_v)
        , m_nb_poles_u(nb_poles_u)
        , m_nb_poles_v(nb_poles_v)
        , m_knots_u(nb_poles_u + degree_u - 1)
        , m_knots_v(nb_poles_v + degree_v - 1)
        , m_poles(nb_poles_u * nb_poles_v, TDimension)
        , m_weights(is_rational ? nb_poles_u * nb_poles_v : 0)
    {
        static_assert(TDimension > 0);
    }

    NurbsSurfaceGeometry(
        const Index degree_u,
        const Index degree_v,
        const Eigen::VectorXd& knots_u,
        const Eigen::VectorXd& knots_v,
        const Poles& poles)
        : m_degree_u(degree_u)
        , m_degree_v(degree_v)
        , m_nb_poles_u(Nurbs::nb_poles(degree_u, length(knots_u)))
        , m_nb_poles_v(Nurbs::nb_poles(degree_v, length(knots_v)))
        , m_knots_u(knots_u)
        , m_knots_v(knots_v)
        , m_poles(poles)
        , m_weights(0)
    {
        static_assert(TDimension > 0);

        if (nb_poles_u() * nb_poles_v() != nb_poles()) {
            throw std::runtime_error("Number of knots and poles do not match");
        }
    }

    NurbsSurfaceGeometry(
        const Index degree_u,
        const Index degree_v,
        const Eigen::VectorXd& knots_u,
        const Eigen::VectorXd& knots_v,
        const Poles& poles,
        const Eigen::VectorXd& weights)
        : m_degree_u(degree_u)
        , m_degree_v(degree_v)
        , m_nb_poles_u(Nurbs::nb_poles(degree_u, length(knots_u)))
        , m_nb_poles_v(Nurbs::nb_poles(degree_v, length(knots_v)))
        , m_knots_u(knots_u)
        , m_knots_v(knots_v)
        , m_poles(poles)
        , m_weights(weights)
    {
        static_assert(TDimension > 0);

        if (nb_poles_u() * nb_poles_v() != nb_poles()) {
            throw std::runtime_error("Number of knots and poles do not match");
        }

        if (nb_poles() != length(weights)) {
            throw std::runtime_error("Number of poles and weights do not match");
        }
    }

    NurbsSurfaceGeometry(
        const Index degree_u,
        const Index degree_v,
        const Eigen::VectorXd& knots_u,
        const Eigen::VectorXd& knots_v,
        const std::vector<ControlPoint>& control_points)
        : m_degree_u(degree_u)
        , m_degree_v(degree_v)
        , m_nb_poles_u(Nurbs::nb_poles(degree_u, length(knots_u)))
        , m_nb_poles_v(Nurbs::nb_poles(degree_v, length(knots_v)))
        , m_knots_u(knots_u)
        , m_knots_v(knots_v)
        , m_poles(length(control_points), TDimension)
        , m_weights(length(control_points))
    {
        static_assert(TDimension > 0);

        if (nb_poles_u() * nb_poles_v() != length(control_points)) {
            throw std::runtime_error("Number of knots and control points do not match");
        }

        for (Index i = 0; i < length(control_points); i++) {
            m_poles.row(i) = std::get<0>(control_points[i]);
            m_weights[i] = std::get<1>(control_points[i]);
        }
    }

public: // static methods
    using SurfaceBase<TDimension>::dimension;

public: // methods
    Index to_single_index(const Index rows, const Index cols, const Index row, const Index col) const
    {
        return row * cols + col;
    }

    Index to_single_index(const Index index_u, const Index index_v) const
    {
        return to_single_index(nb_poles_u(), nb_poles_v(), index_u, index_v);
    }

    std::pair<Index, Index> to_double_index(const Index index) const
    {
        const Index index_u = index / nb_poles_v();
        const Index index_v = index % nb_poles_v();
        return {index_u, index_v};
    }

    Vector pole(const Index index_u, const Index index_v) const
    {
        const Index index = to_single_index(index_u, index_v);
        return pole(index);
    }

    Eigen::Ref<Vector> pole(const Index index_u, const Index index_v)
    {
        const Index index = to_single_index(index_u, index_v);
        return pole(index);
    }

    double weight(const Index index_u, const Index index_v) const
    {
        if (is_rational()) {
            const Index index = to_single_index(index_u, index_v);
            return m_weights[index];
        } else {
            return 1;
        }
    }

    double& weight(const Index index_u, const Index index_v)
    {
        if (is_rational()) {
            const Index index = to_single_index(index_u, index_v);
            return m_weights[index];
        } else {
            throw std::runtime_error("Surface is not rational");
        }
    }

    bool is_rational() const
    {
        return m_weights.size() != 0;
    }

    std::pair<double, double> greville_point(Index index_u, Index index_v) const
    {
        double u = 0.0;
        double v = 0.0;

        for (Index i = 0; i < degree_u(); i++) {
            u += knot_u(index_u + i);
        }

        u /= degree_u();

        for (Index i = 0; i < degree_v(); i++) {
            v += knot_v(index_v + i);
        }

        v /= degree_v();

        return {u, v};
    }

    Pointer<Type> clone()
    {
        Pointer<Type> clone = new_<Type>(
            this->degree_u(), this->degree_v(), this->nb_poles_u(),
            this->nb_poles_v(), this->is_rational());

        for (Index i = 0; i < this->nb_knots_u(); i++) {
            clone->knot_u(i) = this->knot_u(i);
        }

        for (Index i = 0; i < this->nb_knots_v(); i++) {
            clone->knot_v(i) = this->knot_v(i);
        }

        for (Index i = 0; i < this->nb_poles(); i++) {
            clone->pole(i) = this->pole(i);
        }

        if (this->is_rational()) {
            for (Index i = 0; i < this->nb_poles(); i++) {
                clone->weight(i) = this->weight(i);
            }
        }

        return clone;
    }

    Index degree_u() const override
    {
        return m_degree_u;
    }

    Index degree_v() const override
    {
        return m_degree_v;
    }

    Interval domain_u() const override
    {
        double u0 = knot_u(degree_u() - 1);
        double u1 = knot_u(nb_knots_u() - degree_u());

        return Interval(u0, u1);
    }

    Interval domain_v() const override
    {
        double v0 = knot_v(degree_v() - 1);
        double v1 = knot_v(nb_knots_v() - degree_v());

        return Interval(v0, v1);
    }

    template <Index Axis>
    Index nb_knots() const
    {
        static_assert(Axis == 0 || Axis == 1, "Invalid index");

        switch (Axis) {
        case 0:
            return nb_knots_u();
        case 1:
            return nb_knots_v();
        }
    }

    template <Index Axis>
    double knot(const Index index) const
    {
        static_assert(Axis == 0 || Axis == 1, "Invalid index");

        switch (Axis) {
        case 0:
            return knot_u(index);
        case 1:
            return knot_v(index);
        }
    }

    Index nb_knots_u() const
    {
        return length(m_knots_u);
    }

    double knot_u(const Index index) const
    {
        return m_knots_u[index];
    }

    double& knot_u(const Index index)
    {
        return m_knots_u[index];
    }

    Eigen::Ref<const Eigen::VectorXd> knots_u() const
    {
        return m_knots_u;
    }

    Eigen::Ref<Eigen::VectorXd> knots_u()
    {
        return m_knots_u;
    }

    void set_knots_u(Eigen::Ref<const Eigen::VectorXd> value)
    {
        m_knots_u = value;
    }

    Index nb_knots_v() const
    {
        return length(m_knots_v);
    }

    double knot_v(const Index index) const
    {
        return m_knots_v[index];
    }

    double& knot_v(const Index index)
    {
        return m_knots_v[index];
    }

    Eigen::Ref<const Eigen::VectorXd> knots_v() const
    {
        return m_knots_v;
    }

    Eigen::Ref<Eigen::VectorXd> knots_v()
    {
        return m_knots_v;
    }

    void set_knots_v(Eigen::Ref<const Eigen::VectorXd> value)
    {
        m_knots_v = value;
    }

    Index nb_poles_u() const
    {
        return nb_knots_u() - degree_u() + 1;
    }

    Index nb_poles_v() const
    {
        return nb_knots_v() - degree_v() + 1;
    }

    Index nb_poles() const
    {
        return m_poles.rows();
    }

    Vector pole(const Index index) const
    {
        return m_poles.row(index);
    }

    Eigen::Ref<Vector> pole(const Index index)
    {
        return m_poles.row(index);
    }

    Eigen::Ref<const Poles> poles() const
    {
        return m_poles;
    }

    Eigen::Ref<Poles> poles()
    {
        return m_poles;
    }

    void set_poles(Eigen::Ref<const Poles> value)
    {
        m_poles = value;
    }

    double weight(const Index index) const
    {
        return m_weights[index];
    }

    double& weight(const Index index)
    {
        return m_weights[index];
    }

    Eigen::Ref<const Weights> weights() const
    {
        return m_weights;
    }

    Eigen::Ref<Weights> weights()
    {
        return m_weights;
    }

    void set_weights(Eigen::Ref<const Weights> value)
    {
        m_weights = value;
    }

    template <typename TValue, typename TValues>
    TValue evaluate_at(TValues values, const double u, const double v) const
    {
        // compute shape functions

        NurbsSurfaceShapeFunction shape(degree_u(), degree_v(), 0);

        if (is_rational()) {
            shape.compute(
                knots_u(), knots_v(), [&](Index i, Index j) { return weight(i, j); }, u, v);
        } else {
            shape.compute(knots_u(), knots_v(), u, v);
        }

        // compute value

        TValue result;

        for (Index i = 0; i <= degree_u(); i++) {
            for (Index j = 0; j <= degree_v(); j++) {
                Index pole_u = shape.first_nonzero_pole_u() + i;
                Index pole_v = shape.first_nonzero_pole_v() + j;

                TValue value = values(pole_u, pole_v) * shape(0, i, j);

                if (i == 0 && j == 0) {
                    result = value;
                } else {
                    result += value;
                }
            }
        }

        return result;
    }

    template <typename TValue, typename TValues>
    std::vector<TValue> evaluate_at(TValues values, const double u,
        const double v, const Index order) const
    {
        // compute shape functions

        NurbsSurfaceShapeFunction shape(degree_u(), degree_v(), order);

        if (is_rational()) {
            shape.compute(
                knots_u(), knots_v(), [&](Index i, Index j) { return weight(i, j); }, u, v);
        } else {
            shape.compute(knots_u(), knots_v(), u, v);
        }

        // compute derivatives

        const Index nb_shapes = shape.nb_shapes(order);

        std::vector<TValue> result(nb_shapes);

        for (Index k = 0; k < nb_shapes; k++) {
            for (Index i = 0; i <= degree_u(); i++) {
                for (Index j = 0; j <= degree_v(); j++) {
                    const Index pole_u = shape.first_nonzero_pole_u() + i;
                    const Index pole_v = shape.first_nonzero_pole_v() + j;

                    const TValue value = values(pole_u, pole_v) * shape(k, i, j);

                    if (i == 0 && j == 0) {
                        result[k] = value;
                    } else {
                        result[k] += value;
                    }
                }
            }
        }

        return result;
    }

    Vector point_at(const double u, const double v) const override
    {
        auto poles = [&](Index i, Index j) { return pole(i, j); };

        return evaluate_at<Vector>(poles, u, v);
    }

    std::vector<Vector> derivatives_at(const double u, const double v,
        const Index order) const override
    {
        auto poles = [&](Index i, Index j) { return pole(i, j); };

        return evaluate_at<Vector>(poles, u, v, order);
    }

    std::pair<std::vector<Index>, std::vector<Eigen::VectorXd>>
    shape_functions_at(const double u, const double v, const Index order) const
    {
        NurbsSurfaceShapeFunction shape(degree_u(), degree_v(), order);

        if (is_rational()) {
            shape.compute(
                knots_u(), knots_v(), [&](Index i, Index j) -> double {
                    return weight(i, j);
                },
                u, v);
        } else {
            shape.compute(knots_u(), knots_v(), u, v);
        }

        std::vector<Eigen::VectorXd> shapeFunctions(shape.nb_shapes());

        for (Index i = 0; i < shape.nb_shapes(); i++) {
            shapeFunctions[i] = Eigen::VectorXd(shape.nb_nonzero_poles());

            for (Index j = 0; j < shape.nb_nonzero_poles(); j++) {
                shapeFunctions[i][j] = shape(i, j);
            }
        }

        std::vector<Index> indices(shape.nb_nonzero_poles());
        auto it = indices.begin();

        for (Index i = 0; i < shape.nb_nonzero_poles_u(); i++) {
            for (Index j = 0; j < shape.nb_nonzero_poles_v(); j++) {
                Index poleIndex = Math::single_index(nb_poles_u(), nb_poles_v(),
                    shape.first_nonzero_pole_u() + i,
                    shape.first_nonzero_pole_v() + j);

                *(it++) = poleIndex;
            }
        }

        return {indices, shapeFunctions};
    }

    std::vector<Interval> spans_u() const override
    {
        Index first_span = degree_u() - 1;
        Index last_span = nb_knots_u() - degree_u() - 1;

        Index nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (Index i = 0; i < nb_spans; i++) {
            double t0 = knot_u(first_span + i);
            double t1 = knot_u(first_span + i + 1);

            result[i] = Interval(t0, t1);
        }

        return result;
    }

    std::vector<Interval> spans_v() const override
    {
        Index first_span = degree_v() - 1;
        Index last_span = nb_knots_v() - degree_v() - 1;

        Index nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (Index i = 0; i < nb_spans; i++) {
            double t0 = knot_v(first_span + i);
            double t1 = knot_v(first_span + i + 1);

            result[i] = Interval(t0, t1);
        }

        return result;
    }

    void reparametrize(Interval domain_u, Interval domain_v)
    {
        const auto old_domain_u = this->domain_u();

        for (Index i = 0; i < nb_knots_u(); i++) {
            knot_u(i) = domain_u.parameter_at_normalized(old_domain_u.normalized_at(knot_u(i)));
        }

        const auto old_domain_v = this->domain_v();

        for (Index i = 0; i < nb_knots_v(); i++) {
            knot_v(i) = domain_v.parameter_at_normalized(old_domain_v.normalized_at(knot_v(i)));
        }
    }

public: // serialization
    static std::string type_name()
    {
        return "NurbsSurfaceGeometry" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const DataReader reader(source);

        const auto degree_u = reader.read<Index>("degree_u");
        const auto degree_v = reader.read<Index>("degree_v");
        const auto nb_poles_u = reader.read<Index>("nb_poles_u");
        const auto nb_poles_v = reader.read<Index>("nb_poles_v");
        const auto is_rational = reader.has("weights");

        auto result = new_<Type>(degree_u, degree_v, nb_poles_u, nb_poles_v, is_rational);

        reader.fill_vector("knots_u", result->knots_u());
        reader.fill_vector("knots_v", result->knots_v());
        reader.fill_matrix("poles", result->poles());

        if (is_rational) {
            reader.fill_vector("weights", result->weights());
        }

        return result;
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        DataWriter writer(target);

        writer.write("degree_u", data.degree_u());
        writer.write("degree_v", data.degree_v());
        writer.write("nb_poles_u", data.nb_poles_u());
        writer.write("nb_poles_v", data.nb_poles_v());
        writer.write_vector("knots_u", data.knots_u());
        writer.write_vector("knots_v", data.knots_v());
        writer.write_matrix("poles", data.poles());

        if (data.is_rational()) {
            writer.write_vector("weights", data.weights());
        }
    }

public: // python
    static std::string python_name()
    {
        return "NurbsSurfaceGeometry" + std::to_string(dimension()) + "D";
    }

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = NurbsSurfaceGeometry<TDimension>;
        using Base = SurfaceBase<TDimension>;
        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Base, Holder>(m, name.c_str())
            // constructors
            .def(py::init<const Index, const Index, const Index, const Index, const bool>(), "degree_u"_a, "degree_v"_a, "nb_poles_u"_a, "nb_poles_v"_a, "is_rational"_a = false)
            .def(py::init<const Index, const Index, const Eigen::VectorXd, const Eigen::VectorXd, const Poles>(), "degree_u"_a, "degree_v"_a, "knots_u"_a, "knots_v"_a, "poles"_a)
            .def(py::init<const Index, const Index, const Eigen::VectorXd, const Eigen::VectorXd, const Poles, const Eigen::VectorXd>(), "degree_u"_a, "degree_v"_a, "knots_u"_a, "knots_v"_a, "poles"_a, "weights"_a)
            .def(py::init<const Index, const Index, const Eigen::VectorXd, const Eigen::VectorXd, const std::vector<ControlPoint>>(), "degree_u"_a, "degree_v"_a, "knots_u"_a, "knots_v"_a, "control_points"_a)
            // read-only properties
            .def_property_readonly("is_rational", &Type::is_rational)
            .def_property_readonly("nb_knots_u", &Type::nb_knots_u)
            .def_property_readonly("nb_knots_v", &Type::nb_knots_v)
            .def_property_readonly("nb_poles", &Type::nb_poles)
            .def_property_readonly("nb_poles_u", &Type::nb_poles_u)
            .def_property_readonly("nb_poles_v", &Type::nb_poles_v)
            // properties
            .def_property("knots_u", py::overload_cast<>(&Type::knots_u), &Type::set_knots_u)
            .def_property("knots_v", py::overload_cast<>(&Type::knots_v), &Type::set_knots_v)
            .def_property("poles", py::overload_cast<>(&Type::poles), &Type::set_poles)
            .def_property("weights", py::overload_cast<>(&Type::weights), &Type::set_weights)
            // methods
            .def("clone", &Type::clone)
            .def("knot_u", py::overload_cast<Index>(&Type::knot_u), "index"_a)
            .def("knot_v", py::overload_cast<Index>(&Type::knot_v), "index"_a)
            .def("pole", (Vector(Type::*)(const Index) const) & Type::pole, "index"_a)
            .def("pole", (Vector(Type::*)(const Index, const Index) const) & Type::pole, "index_u"_a, "index_v"_a)
            .def("shape_functions_at", &Type::shape_functions_at, "u"_a, "v"_a, "order"_a)
            .def("weight", (double (Type::*)(const Index) const) & Type::weight, "index"_a)
            .def("weight", (double (Type::*)(const Index, const Index) const) & Type::weight, "index_u"_a, "index_v"_a)
            .def("greville_point", &Type::greville_point, "index_u"_a, "index_v"_a);

        m.def("add", [](Model& model, Holder data) { model.add<Type>(data); });

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs
