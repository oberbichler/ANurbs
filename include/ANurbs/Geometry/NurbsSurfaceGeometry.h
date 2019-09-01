#pragma once

#include "../Define.h"

#include "SurfaceBase.h"

#include "../Algorithm/Nurbs.h"
#include "../Algorithm/NurbsSurfaceShapeFunction.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <stdexcept>
#include <vector>

namespace ANurbs {

template <Index TDimension>
class NurbsSurfaceGeometry : public SurfaceBase<TDimension>
{
public:     // types
    using Type = NurbsSurfaceGeometry<TDimension>;
    using Vector = typename SurfaceBase<TDimension>::Vector;

private:    // variables
    Index m_degree_u;
    Index m_degree_v;
    Index m_nb_poles_u;
    Index m_nb_poles_v;
    std::vector<double> m_knots_u;
    std::vector<double> m_knots_v;
    std::vector<Vector> m_poles;
    std::vector<double> m_weights;

public:     // constructor
    NurbsSurfaceGeometry(const Index degree_u, const Index degree_v,
        const Index nb_poles_u, const Index nb_poles_v, const bool is_rational)
        : m_degree_u(degree_u), m_degree_v(degree_v), m_nb_poles_u(nb_poles_u),
        m_nb_poles_v(nb_poles_v), m_knots_u(nb_poles_u + degree_u - 1),
        m_knots_v(nb_poles_v + degree_v - 1), m_poles(nb_poles_u * nb_poles_v),
        m_weights(is_rational ? nb_poles_u * nb_poles_v : 0)
    {
    }

public:     // static methods
    using SurfaceBase<TDimension>::dimension;

public:     // methods
    Index to_single_index(const Index index_u, const Index index_v) const
    {
        return index_u * nb_poles_v() + index_v;
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

    Vector& pole(const Index index_u, const Index index_v)
    {
        const Index index = to_single_index(index_u, index_v);
        return pole(index);
    }

    void set_pole(const Index index_u, const Index index_v, const Vector& value)
    {
        const Index index = to_single_index(index_u, index_v);
        m_poles[index] = value;
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
            throw std::runtime_error("");
        }
    }

    void set_weight(const Index index_u, const Index index_v, const double value)
    {
        if (is_rational()) {
            const Index index = to_single_index(index_u, index_v);
            m_weights[index] = value;
        } else {
            throw std::invalid_argument("Geometry is not rational");
        }
    }

    bool is_rational() const
    {
        return m_weights.size() != 0;
    }

    Pointer<Type> clone()
    {
        Pointer<Type> clone = new_<Type>(
            this->degree_u(), this->degree_v(), this->nb_poles_u(),
            this->nb_poles_v(), this->is_rational());

        for (Index i = 0; i < this->nb_knots_u(); i++) {
            clone->set_knot_u(i, this->knot_u(i));
        }

        for (Index i = 0; i < this->nb_knots_v(); i++) {
            clone->set_knot_v(i, this->knot_v(i));
        }

        for (Index i = 0; i < this->nb_poles(); i++) {
            clone->set_pole(i, this->pole(i));
        }

        if (this->is_rational()) {
            for (Index i = 0; i < this->nb_poles(); i++) {
                clone->set_weight(i, this->weight(i));
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
        return static_cast<Index>(m_knots_u.size());
    }

    double knot_u(const Index index) const
    {
        return m_knots_u[index];
    }

    void set_knot_u(const Index index, const double value)
    {
        m_knots_u[index] = value;
    }

    const std::vector<double>& knots_u() const
    {
        return m_knots_u;
    }

    Index nb_knots_v() const
    {
        return static_cast<Index>(m_knots_v.size());
    }

    double knot_v(const Index index) const
    {
        return m_knots_v[index];
    }

    void set_knot_v(const Index index, const double value)
    {
        m_knots_v[index] = value;
    }

    const std::vector<double>& knots_v() const
    {
        return m_knots_v;
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
        return nb_poles_u() * nb_poles_v();
    }

    Vector pole(const Index index) const
    {
        return m_poles[index];
    }

    Vector& pole(const Index index)
    {
        return m_poles[index];
    }

    void set_pole(const Index index, const Vector& value)
    {
        m_poles[index] = value;
    }

    const std::vector<Vector>& poles() const
    {
        return m_poles;
    }

    double weight(const Index index) const
    {
        return m_weights[index];
    }

    double& weight(const Index index)
    {
        return m_weights[index];
    }

    void set_weight(const Index index, const double value)
    {
        const Index index_u = index / nb_poles_v();
        const Index index_v = index % nb_poles_v();

        set_weight(index_u, index_v, value);
    }

    const std::vector<double>& weights() const
    {
        return m_weights;
    }

    template <typename TValue, typename TValues>
    TValue evaluate_at(TValues values, const double u, const double v) const
    {
        // compute shape functions

        NurbsSurfaceShapeFunction shape(degree_u(), degree_v(), 0);

        if (is_rational()) {
            shape.compute(knots_u(), knots_v(), [&](Index i, Index j) {
            return weight(i, j); }, u, v);
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
            shape.compute(knots_u(), knots_v(), [&](Index i, Index j) {
                return weight(i, j); }, u, v);
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

                    const TValue value =
                        values(pole_u, pole_v) * shape(k, i, j);

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

    std::pair<std::vector<Index>, std::vector<std::vector<double>>>
    shape_functions_at(const double u, const double v, const Index order) const
    {
        NurbsSurfaceShapeFunction shape(degree_u(), degree_v(), order);

        if (is_rational()) {
            shape.compute(knots_u(), knots_v(), [&](Index i, Index j) -> double {
                return weight(i, j);
            }, u, v);
        } else {
            shape.compute(knots_u(), knots_v(), u, v);
        }

        std::vector<std::vector<double>> shapeFunctions(shape.nb_shapes());

        for (Index i = 0; i < shape.nb_shapes(); i++) {
            shapeFunctions[i] = std::vector<double>(shape.nb_nonzero_poles());

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

        for (auto& knot : m_knots_u) {
            knot = domain_u.parameter_at_normalized(old_domain_u.normalized_at(knot));
        }

        const auto old_domain_v = this->domain_v();

        for (auto& knot : m_knots_v) {
            knot = domain_u.parameter_at_normalized(old_domain_v.normalized_at(knot));
        }
    }

public:     // serialization
    static std::string type_name()
    {
        return "NurbsSurfaceGeometry" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const auto poles = source.at("Poles");
        const auto knots_u = source.at("KnotsU");
        const auto knots_v = source.at("KnotsV");
        const auto weights = source.value("Weights", std::vector<double>());
        
        const Index degree_u = source.at("DegreeU");
        const Index degree_v = source.at("DegreeV");
        const Index nb_poles_u = source.at("NbPolesU");
        const Index nb_poles_v = source.at("NbPolesV");
        const bool is_rational = !weights.empty();

        auto result = new_<Type>(degree_u, degree_v, nb_poles_u, nb_poles_v,
            is_rational);

        for (Index i = 0; i < knots_u.size(); i++) {
            result->set_knot_u(i, knots_u[i]);
        }

        for (Index i = 0; i < knots_v.size(); i++) {
            result->set_knot_v(i, knots_v[i]);
        }

        for (Index i = 0; i < poles.size(); i++) {
            result->set_pole(i, poles[i]);
        }

        if (is_rational) {
            for (Index i = 0; i < weights.size(); i++) {
                result->set_weight(i, weights[i]);
            }
        }

        return result;
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        target["DegreeU"] = data.degree_u();
        target["DegreeV"] = data.degree_v();
        target["NbPolesU"] = data.nb_poles_u();
        target["NbPolesV"] = data.nb_poles_v();
        target["KnotsU"] = data.knots_u();
        target["KnotsV"] = data.knots_v();
        target["Poles"] = ToJson(data.poles());

        if (data.is_rational()) {
            target["Weights"] = ToJson(data.weights());
        }
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = NurbsSurfaceGeometry<TDimension>;
        using Base = SurfaceBase<TDimension>;
        using Holder = Pointer<Type>;

        const std::string name = Type::type_name();

        py::class_<Type, Base, Holder>(m, name.c_str())
            // constructors
            .def(py::init<const Index, const Index, const Index, const Index,
                const bool>(), "degree_u"_a, "degree_v"_a, "nb_poles_u"_a,
                "nb_poles_v"_a, "is_rational"_a=false)
            // read-only properties
            .def_property_readonly("is_rational", &Type::is_rational)
            .def_property_readonly("knots_u", &Type::knots_u)
            .def_property_readonly("knots_v", &Type::knots_v)
            .def_property_readonly("nb_knots_u", &Type::nb_knots_u)
            .def_property_readonly("nb_knots_v", &Type::nb_knots_v)
            .def_property_readonly("nb_poles", &Type::nb_poles)
            .def_property_readonly("nb_poles_u", &Type::nb_poles_u)
            .def_property_readonly("nb_poles_v", &Type::nb_poles_v)
            .def_property_readonly("poles", &Type::poles)
            .def_property_readonly("weights", &Type::weights)
            // methods
            // .def("Poles", &Type::poles)
            // .def("Weights", &Type::weights)
            .def("clone", &Type::clone)
            .def("knot_u", &Type::knot_u, "index"_a)
            .def("knot_v", &Type::knot_v, "index"_a)
            .def("pole", (Vector (Type::*)(const Index) const) &Type::pole,
                "index"_a)
            .def("pole", (Vector (Type::*)(const Index, const Index) const)
                &Type::pole, "index_u"_a, "index_v"_a)
            .def("set_knot_u", &Type::set_knot_u, "index"_a, "value"_a)
            .def("set_knot_v", &Type::set_knot_v, "index"_a, "value"_a)
            .def("set_pole", (void (Type::*)(const Index, const Vector&))
                &Type::set_pole, "index"_a, "value"_a)
            .def("set_pole", (void (Type::*)(const Index, const Index,
                const Vector&)) &Type::set_pole, "index_u"_a, "index_v"_a,
                "value"_a)
            .def("set_weight", (void (Type::*)(const Index, const double))
                &Type::set_weight, "index"_a, "value"_a)
            .def("set_weight", (void (Type::*)(const Index, const Index,
                const double)) &Type::set_weight, "index_u"_a, "index_v"_a,
                "value"_a)
            .def("shape_functions_at", &Type::shape_functions_at, "u"_a, "v"_a,
                "order"_a)
            .def("weight", (double (Type::*)(const Index) const) &Type::weight,
                "index"_a)
            .def("weight", (double (Type::*)(const Index, const Index) const)
                &Type::weight, "index_u"_a, "index_v"_a)
        ;

        m.def("add", [](Model& model, Holder data) { model.add<Type>(data); } );

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace ANurbs
