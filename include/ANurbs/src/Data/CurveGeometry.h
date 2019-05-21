#pragma once

#include "../../Model/Json.h"
#include "../../Model/Model.h"
#include "../../Model/Ref.h"

#include <ANurbs/ANurbs.h>

#include <vector>

namespace ANurbs {

template <int TDimension>
struct DataIO<NurbsCurveGeometry<TDimension>>
{
    using DataType = NurbsCurveGeometry<TDimension>;

    static std::string
    Type()
    {
        return "NurbsCurveGeometry" + std::to_string(TDimension) + "D";
    }

    static Unique<DataType>
    Load(
        Model& model,
        const Json& source)
    {
        // const auto poles = source.at("Poles");
        // const auto knots = source.at("Knots");
        // const auto weights = source.value("Weights", std::vector<double>());
        
        // const int degree = source.at("Degree");
        // const int nbPoles = static_cast<int>(poles.size());
        // const bool isRational = !weights.empty();

        // auto result = New<DataType>(degree, nbPoles, isRational);

        // for (int i = 0; i < knots.size(); i++) {
        //     result->SetKnot(i, knots[i]);
        // }

        // for (int i = 0; i < nbPoles; i++) {
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

    static void
    Save(
        const Model& model,
        const DataType& data,
        Json& target)
    {
        // target["Degree"] = data.degree();
        // target["Knots"] = data.knots();
        // target["NbPoles"] = data.nb_poles();
        // target["Poles"] = ToJson(data.poles());

        // if (data.IsRational()) {
        //     target["Weights"] = data.weights();
        // }
    }
};

} // namespace ANurbs