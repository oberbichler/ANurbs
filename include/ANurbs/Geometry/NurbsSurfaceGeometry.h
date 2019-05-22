#pragma once

#include "../Define.h"

#include "SurfaceBase.h"

#include "../Algorithm/KnotVector.h"
#include "../Algorithm/NurbsSurfaceShapeFunction.h"

#include "../Model/CadAttributes.h"
#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <stdexcept>
#include <vector>

namespace ANurbs {

template <int TDimension>
class NurbsSurfaceGeometry : public SurfaceBase<TDimension>
{
public:     // types
    using Type = NurbsSurfaceGeometry<TDimension>;
    using Vector = typename SurfaceBase<TDimension>::Vector;

private:    // variables
    int m_degree_u;
    int m_degree_v;
    int m_nb_poles_u;
    int m_nb_poles_v;
    std::vector<double> m_knots_u;
    std::vector<double> m_knots_v;
    std::vector<Vector> m_poles;
    std::vector<double> m_weights;

public:     // constructor
    NurbsSurfaceGeometry(const int degree_u, const int degree_v,
        const int nb_poles_u, const int nb_poles_v, const bool is_rational)
        : m_degree_u(degree_u), m_degree_v(degree_v), m_nb_poles_u(nb_poles_u),
        m_nb_poles_v(nb_poles_v), m_knots_u(nb_poles_u + degree_u - 1),
        m_knots_v(nb_poles_v + degree_v - 1), m_poles(nb_poles_u * nb_poles_v),
        m_weights(is_rational ? nb_poles_u * nb_poles_v : 0)
    {
    }

public:     // static methods
    using SurfaceBase<TDimension>::dimension;

public:     // methods
    int to_single_index(const int index_u, const int index_v) const
    {
        return index_u * nb_poles_v() + index_v;
    }

    std::pair<int, int> to_double_index(const int index) const
    {
        const int index_u = index / nb_poles_v();
        const int index_v = index % nb_poles_v();
        return {index_u, index_v};
    }

    Vector pole(const int index_u, const int index_v) const
    {
        const int index = to_single_index(index_u, index_v);
        return pole(index);
    }

    Vector& pole(const int index_u, const int index_v)
    {
        const int index = to_single_index(index_u, index_v);
        return pole(index);
    }

    void set_pole(const int index_u, const int index_v, const Vector& value)
    {
        const int index = to_single_index(index_u, index_v);
        m_poles[index] = value;
    }

    double weight(const int index_u, const int index_v) const
    {
        if (is_rational()) {
            const int index = to_single_index(index_u, index_v);
            return m_weights[index];
        } else {
            return 1;
        }
    }

    double& weight(const int index_u, const int index_v)
    {
        if (is_rational()) {
            const int index = to_single_index(index_u, index_v);
            return m_weights[index];
        } else {
            throw std::runtime_error("");
        }
    }

    void set_weight(const int index_u, const int index_v, const double value)
    {
        if (is_rational()) {
            const int index = to_single_index(index_u, index_v);
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

        for (int i = 0; i < this->nb_knots_u(); i++) {
            clone->set_knot_u(i, this->knot_u(i));
        }

        for (int i = 0; i < this->nb_knots_v(); i++) {
            clone->set_knot_v(i, this->knot_v(i));
        }

        for (int i = 0; i < this->nb_poles(); i++) {
            clone->set_pole(i, this->pole(i));
        }

        if (this->is_rational()) {
            for (int i = 0; i < this->nb_poles(); i++) {
                clone->set_weight(i, this->weight(i));
            }
        }

        return clone;
    }

    int degree_u() const override
    {
        return m_degree_u;
    }

    int degree_v() const override
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

    template <int Axis>
    int nb_knots() const
    {
        static_assert(Axis == 0 || Axis == 1, "Invalid index");

        switch (Axis) {
        case 0:
            return nb_knots_u();
        case 1:
            return nb_knots_v();
        }
    }

    template <int Axis>
    double knot(const int index) const
    {
        static_assert(Axis == 0 || Axis == 1, "Invalid index");

        switch (Axis) {
        case 0:
            return knot_u(index);
        case 1:
            return knot_v(index);
        }
    }

    int nb_knots_u() const
    {
        return static_cast<int>(m_knots_u.size());
    }

    double knot_u(const int index) const
    {
        return m_knots_u[index];
    }

    void set_knot_u(const int index, const double value)
    {
        m_knots_u[index] = value;
    }

    const std::vector<double>& knots_u() const
    {
        return m_knots_u;
    }

    int nb_knots_v() const
    {
        return static_cast<int>(m_knots_v.size());
    }

    double knot_v(const int index) const
    {
        return m_knots_v[index];
    }

    void set_knot_v(const int index, const double value)
    {
        m_knots_v[index] = value;
    }

    const std::vector<double>& knots_v() const
    {
        return m_knots_v;
    }

    int nb_poles_u() const
    {
        return nb_knots_u() - degree_u() + 1;
    }

    int nb_poles_v() const
    {
        return nb_knots_v() - degree_v() + 1;
    }

    int nb_poles() const
    {
        return nb_poles_u() * nb_poles_v();
    }

    Vector pole(const int index) const
    {
        return m_poles[index];
    }

    Vector& pole(const int index)
    {
        return m_poles[index];
    }

    void set_pole(const int index, const Vector& value)
    {
        m_poles[index] = value;
    }

    const std::vector<Vector>& poles() const
    {
        return poles;
    }

    double weight(const int index) const
    {
        return m_weights[index];
    }

    double& weight(const int index)
    {
        return m_weights[index];
    }

    void set_weight(const int index, const double value)
    {
        const int indexU = index / nb_poles_v();
        const int indexV = index % nb_poles_v();

        set_weight(indexU, indexV, value);
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
            shape.compute(knots_u(), knots_v(), [&](int i, int j) -> double { return weight(i, j); }, u, v);
        } else {
            shape.compute(knots_u(), knots_v(), u, v);
        }

        // compute value

        TValue result;

        for (int i = 0; i <= degree_u(); i++) {
            for (int j = 0; j <= degree_v(); j++) {
                int poleU = shape.first_nonzero_pole_u() + i;
                int poleV = shape.first_nonzero_pole_v() + j;

                TValue value = values(poleU, poleV) * shape(0, i, j);

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
        const double v, const int order) const
    {
        // compute shape functions

        NurbsSurfaceShapeFunction shape(degree_u(), degree_v(), order);

        if (is_rational()) {
            shape.compute(knots_u(), knots_v(), [&](int i, int j) {
                return weight(i, j); }, u, v);
        } else {
            shape.compute(knots_u(), knots_v(), u, v);
        }

        // compute derivatives

        int nbShapes = shape.nb_shapes(order);

        std::vector<TValue> result(nbShapes);

        for (int k = 0; k < nbShapes; k++) {
            for (int i = 0; i <= degree_u(); i++) {
                for (int j = 0; j <= degree_v(); j++) {
                    int poleU = shape.first_nonzero_pole_u() + i;
                    int poleV = shape.first_nonzero_pole_v() + j;

                    TValue value = values(poleU, poleV) * shape(k, i, j);

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
        auto poles = [&](int i, int j) -> Vector { return pole(i, j); };

        return evaluate_at<Vector>(poles, u, v);
    }

    std::vector<Vector> derivatives_at(const double u, const double v,
        const int order) const override
    {
        auto poles = [&](int i, int j) -> Vector { return pole(i, j); };

        return evaluate_at<Vector>(poles, u, v, order);
    }

    std::pair<std::vector<int>, std::vector<std::vector<double>>>
    shape_functions_at(const double u, const double v, const int order) const
    {
        NurbsSurfaceShapeFunction shape(degree_u(), degree_v(), order);

        if (is_rational()) {
            shape.compute(knots_u(), knots_v(), [&](int i, int j) -> double {
                return weight(i, j);
            }, u, v);
        } else {
            shape.compute(knots_u(), knots_v(), u, v);
        }

        std::vector<std::vector<double>> shapeFunctions(shape.nb_shapes());

        for (int i = 0; i < shape.nb_shapes(); i++) {
            shapeFunctions[i] = std::vector<double>(shape.nb_nonzero_poles());

            for (int j = 0; j < shape.nb_nonzero_poles(); j++) {
                shapeFunctions[i][j] = shape(i, j);
            }
        }

        std::vector<int> indices(shape.nb_nonzero_poles());
        auto it = indices.begin();

        for (int i = 0; i < shape.nb_nonzero_poles_u(); i++) {
            for (int j = 0; j < shape.nb_nonzero_poles_v(); j++) {
                int poleIndex = Math::matrix_index(nb_poles_u(), nb_poles_v(),
                    shape.first_nonzero_pole_u() + i,
                    shape.first_nonzero_pole_v() + j);

                *(it++) = poleIndex;
            }
        }

        return {indices, shapeFunctions};
    }

    std::vector<Interval> spans_u() const override
    {
        int first_span = degree_u() - 1;
        int last_span = nb_knots_u() - degree_u() - 1;

        int nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (int i = 0; i < nb_spans; i++) {
            double t0 = knot_u(first_span + i);
            double t1 = knot_u(first_span + i + 1);

            result[i] = Interval(t0, t1);
        }

        return result;
    }

    std::vector<Interval> spans_v() const override
    {
        int first_span = degree_v() - 1;
        int last_span = nb_knots_v() - degree_v() - 1;

        int nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (int i = 0; i < nb_spans; i++) {
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
    using Attributes = Attributes;

    static std::string type_name()
    {
        return "NurbsSurfaceGeometry" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        // const auto poles = source.at("Poles");
        // const auto knotsU = source.at("KnotsU");
        // const auto knotsV = source.at("KnotsV");
        // const auto weights = source.value("Weights", std::vector<double>());
        
        // const int degreeU = source.at("DegreeU");
        // const int degreeV = source.at("DegreeV");
        // const int nbPolesU = source.at("NbPolesU");
        // const int nbPolesV = source.at("NbPolesV");
        // const bool isRational = !weights.empty();

        // auto result = new_<Type>(degreeU, degreeV, nbPolesU, nbPolesV,
        //     isRational);

        // for (int i = 0; i < knotsU.size(); i++) {
        //     result->SetKnotU(i, knotsU[i]);
        // }

        // for (int i = 0; i < knotsV.size(); i++) {
        //     result->SetKnotV(i, knotsV[i]);
        // }

        // for (int i = 0; i < poles.size(); i++) {
        //     result->SetPole(i, poles[i]);
        // }

        // if (isRational) {
        //     for (int i = 0; i < weights.size(); i++) {
        //         result->SetWeight(i, weights[i]);
        //     }
        // }

        // return result;
        return nullptr;
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
    //     target["DegreeU"] = data.DegreeU();
    //     target["DegreeV"] = data.DegreeV();
    //     target["NbPolesU"] = data.NbPolesU();
    //     target["NbPolesV"] = data.NbPolesV();
    //     target["KnotsU"] = data.KnotsU();
    //     target["KnotsV"] = data.KnotsV();
    //     target["Poles"] = ToJson(data.Poles());

    //     if (data.IsRational()) {
    //         target["Weights"] = ToJson(data.Weights());
    //     }
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
            .def(py::init<const int, const int, const int, const int,
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
            // methods
            // .def("Poles", &Type::poles)
            // .def("Weights", &Type::weights)
            .def("clone", &Type::clone)
            .def("knot_u", &Type::knot_u, "index"_a)
            .def("knot_v", &Type::knot_v, "index"_a)
            .def("pole", (Vector (Type::*)(const int) const) &Type::pole,
                "index"_a)
            .def("pole", (Vector (Type::*)(const int, const int) const)
                &Type::pole, "indexU"_a, "indexV"_a)
            .def("set_knot_u", &Type::set_knot_u, "index"_a, "value"_a)
            .def("set_knot_v", &Type::set_knot_v, "index"_a, "value"_a)
            .def("set_pole", (void (Type::*)(const int, const Vector&))
                &Type::set_pole, "index"_a, "value"_a)
            .def("set_pole", (void (Type::*)(const int, const int,
                const Vector&)) &Type::set_pole, "indexU"_a, "indexV"_a,
                "value"_a)
            .def("set_weight", (void (Type::*)(const int, const double))
                &Type::set_weight, "index"_a, "value"_a)
            .def("set_weight", (void (Type::*)(const int, const int,
                const double)) &Type::set_weight, "indexU"_a, "indexV"_a,
                "value"_a)
            .def("shape_functions_at", &Type::shape_functions_at, "u"_a, "v"_a,
                "order"_a)
            .def("spans_u", &Type::spans_u)
            .def("spans_v", &Type::spans_v)
            .def("weight", (double (Type::*)(const int) const) &Type::weight,
                "index"_a)
            .def("weight", (double (Type::*)(const int, const int) const)
                &Type::weight, "indexU"_a, "indexV"_a)
        ;

        // RegisterDataType<Type>(m, model, name);
    }
};

} // namespace ANurbs
