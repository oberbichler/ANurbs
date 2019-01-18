#pragma once

#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/Core>

#include <vector>

namespace ANurbs {

template <typename TVector>
struct DataIO<CurveGeometry<TVector>>
{
    using DataType = CurveGeometry<TVector>;

    static std::string
    Type()
    {
        return "NurbsCurveGeometry" + std::to_string(DimensionOf<TVector>()) + 
            "D";
    }

    static Unique<DataType>
    Load(
        Model& model,
        const Json& source)
    {
        const auto poles = source.at("Poles");
        const auto knots = source.at("Knots");
        const auto weights = source.value("Weights", std::vector<double>());
        
        const int degree = source.at("Degree");
        const int nbPoles = static_cast<int>(poles.size());
        const bool isRational = !weights.empty();

        auto result = New<DataType>(degree, nbPoles, isRational);

        for (int i = 0; i < knots.size(); i++) {
            result->SetKnot(i, knots[i]);
        }

        for (int i = 0; i < nbPoles; i++) {
            result->SetPole(i, poles[i]);
        }

        if (isRational) {
            for (int i = 0; i < weights.size(); i++) {
                result->SetWeight(i, weights[i]);
            }
        }

        return result;
    }

    static void
    Save(
        const Model& model,
        const DataType& data,
        Json& target)
    {
        target["Degree"] = data.Degree();
        target["Knots"] = data.Knots();
        target["Poles"] = ToJson(data.Poles());

        if (data.IsRational()) {
            target["Weights"] = data.Weights();
        }
    }
};

} // namespace ANurbs