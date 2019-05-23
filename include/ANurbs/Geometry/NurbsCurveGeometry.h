#pragma once

#include "../Define.h"

#include "CurveBase.h"

#include "../Algorithm/Nurbs.h"
#include "../Algorithm/NurbsCurveShapeFunction.h"

#include "../Model/CadAttributes.h"
#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <stdexcept>
#include <vector>

namespace ANurbs {

template <int TDimension>
struct NurbsCurveGeometry : public CurveBase<TDimension>
{
public:     // types
    using Type = NurbsCurveGeometry<TDimension>;
    using Vector = typename CurveBase<TDimension>::Vector;

private:    // variables
    const int m_degree;
    std::vector<double> m_knots;
    std::vector<Vector> m_poles;
    std::vector<double> m_weights;

public:     // constructors
    NurbsCurveGeometry(const int degree, int nb_poles, bool is_rational)
        : m_degree(degree), m_poles(nb_poles), m_weights(is_rational ?
        nb_poles : 0), m_knots(Nurbs::nb_knots(degree, nb_poles))
    {
        static_assert(TDimension > 0);
    }

    NurbsCurveGeometry(const int degree, const std::vector<double>& knots,
        const std::vector<Vector>& poles)
        : m_degree(degree), m_knots(knots), m_poles(poles), m_weights()
    {
        static_assert(TDimension > 0);

        if (knots.size() != Nurbs::nb_knots(degree, poles.size())) {
            throw std::runtime_error("Number of knots and poles do not match");
        }
    }

    NurbsCurveGeometry(const int degree, const std::vector<double>& knots,
        const std::vector<Vector>& poles, const std::vector<double>& weights)
        : m_degree(degree), m_knots(knots), m_poles(poles), m_weights(weights)
    {
        static_assert(TDimension > 0);

        if (knots.size() != Nurbs::nb_knots(degree, poles.size())) {
            throw std::runtime_error("Number of knots and poles do not match");
        }

        if (weights.size() != poles.size()) {
            throw std::runtime_error(
                "Number of poles and weights do not match");
        }
    }

    // FIXME: constructor with Vector<TDimension + 1>

public:     // static methods
    using CurveBase<TDimension>::dimension;

public:     // methods
    int degree() const override
    {
        return m_degree;
    }
    
    std::vector<Vector> derivatives_at(const double t, const int order)
        const override
    {
        NurbsCurveShapeFunction shape_function;

        shape_function.resize(m_degree, order);

        if (m_weights.size() > 0) {
            shape_function.compute(m_knots, [&](int i) { return weight(i); }, t);
        } else {
            shape_function.compute(m_knots, t);
        }

        std::vector<Vector> derivatives(shape_function.nb_shapes());

        for (int order = 0; order < shape_function.nb_shapes(); order++) {
            for (int i = 0; i < shape_function.nb_nonzero_poles(); i++) {
                int index = shape_function.first_nonzero_pole() + i;

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

    double knot(int i) const
    {
        return m_knots[i];
    }

    const std::vector<double>& knots() const
    {
        return m_knots;
    }

    int nb_knots() const
    {
        return static_cast<int>(m_knots.size());
    }

    int nb_poles() const
    {
        return static_cast<int>(m_poles.size());
    }

    Vector point_at(const double t) const override
    {
        NurbsCurveShapeFunction shape_function;

        shape_function.resize(m_degree, 0);

        if (m_weights.size() > 0) {
            shape_function.compute(m_knots, [&](int i) { return weight(i); }, t);
        } else {
            shape_function.compute(m_knots, t);
        }

        Vector point;

        for (int i = 0; i < shape_function.nb_nonzero_poles(); i++) {
            const int index = shape_function.first_nonzero_pole() + i;

            if (i == 0) {
                point = pole(index) * shape_function(0, i);
            } else {
                point += pole(index) * shape_function(0, i);
            }
        }

        return point;
    }

    Vector pole(int i) const
    {
        return m_poles.at(i);
    }

    const std::vector<Vector>& poles() const
    {
        return m_poles;
    }
    
    void set_knot(int i, double value)
    {
        m_knots[i] = value;
    }
    
    void set_pole(int i, Vector value)
    {
        m_poles.at(i) = value;
    }
    
    void set_weight(int i, double value)
    {
        m_weights.at(i) = value;
    }

    std::pair<std::vector<int>, MatrixXd> shape_functions_at(const double t,
        const int order) const
    {
        NurbsCurveShapeFunction shape_function(degree(), order);

        if (is_rational()) {
            shape_function.compute(knots(), [&](int i) {
                return weight(i); }, t);
        } else {
            shape_function.compute(knots(), t);
        }

        MatrixXd values(shape_function.nb_shapes(),
            shape_function.nb_nonzero_poles());

        for (int i = 0; i < shape_function.nb_shapes(); i++) {
            for (int j = 0; j < shape_function.nb_nonzero_poles(); j++) {
                values(i, j) = shape_function(i, j);
            }
        }

        return {shape_function.nonzero_pole_indices(), values};
    }

    std::vector<Interval> spans() const override
    {
        const int first_span = degree() - 1;
        const int last_span = nb_knots() - degree() - 1;

        const int nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (int i = 0; i < nb_spans; i++) {
            const double t0 = knot(first_span + i);
            const double t1 = knot(first_span + i + 1);

            result[i] = Interval(t0, t1);
        }

        return result;
    }
    
    double weight(int i) const
    {
        return m_weights.at(i);
    }
    
    const std::vector<double>& weights() const
    {
        return m_weights;
    }

public:     // serialization
    using Attributes = Attributes;

    static std::string type_name()
    {
        return "NurbsCurveGeometry" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const auto poles = source.at("Poles");
        const auto knots = source.at("Knots");
        const auto weights = source.value("Weights", std::vector<double>());
        
        const int degree = source.at("Degree");
        const int nb_poles = static_cast<int>(poles.size());
        const bool is_rational = !weights.empty();

        auto result = new_<Type>(degree, nb_poles, is_rational);

        for (int i = 0; i < knots.size(); i++) {
            result->set_knot(i, knots[i]);
        }

        for (int i = 0; i < nb_poles; i++) {
            result->set_pole(i, poles[i]);
        }

        if (is_rational) {
            for (int i = 0; i < weights.size(); i++) {
                result->set_weight(i, weights[i]);
            }
        }

        return result;
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        target["Degree"] = data.degree();
        target["Knots"] = data.knots();
        target["NbPoles"] = data.nb_poles();
        target["Poles"] = ToJson(data.poles());

        if (data.is_rational()) {
            target["Weights"] = data.weights();
        }
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = NurbsCurveGeometry<TDimension>;
        using Base = CurveBase<TDimension>; 
        using Holder = Pointer<Type>;

        const std::string name = Type::type_name();

        py::class_<Type, Base, Holder>(m, name.c_str())
            // constructors
            .def(py::init<const int, const int, const bool>(), "degree"_a,
                "nb_poles"_a, "is_rational"_a)
            .def(py::init<const int, const std::vector<double>,
                const std::vector<Vector>>(), "degree"_a, "knots"_a, "poles"_a)
            .def(py::init<const int, const std::vector<double>,
                const std::vector<Vector>, const std::vector<double>>(),
                "degree"_a, "knots"_a, "poles"_a, "weights"_a)
            // read-only properties
            .def_property_readonly("is_rational", &Type::is_rational)
            .def_property_readonly("nb_knots", &Type::nb_knots)
            .def_property_readonly("nb_poles", &Type::nb_poles)
            // methods
            .def("knot", &Type::knot, "index"_a)
            .def("knots", &Type::knots)
            .def("poles", &Type::poles)
            .def("set_knot", &Type::set_knot, "index"_a, "value"_a)
            .def("set_pole", &Type::set_pole, "index"_a, "value"_a)
            .def("set_weight", &Type::set_weight, "index"_a, "value"_a)
            .def("shape_functions_at", &Type::shape_functions_at, "t"_a,
                "order"_a)
            .def("weight", &Type::weight, "index"_a)
            .def("weights", &Type::weights)
            // .def("clone", &Type::clone)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace ANurbs