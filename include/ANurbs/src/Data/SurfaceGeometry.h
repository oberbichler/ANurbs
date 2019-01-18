#pragma once

#include "Json.h"
#include "Model.h"
#include "Ref.h"

#include <ANurbs/Core>

#include <vector>

namespace ANurbs {

template <typename TVector>
struct DataIO<SurfaceGeometry<TVector>>
{
    using DataType = SurfaceGeometry<TVector>;

    static std::string
    Type()
    {
        return "NurbsSurfaceGeometry" + std::to_string(DimensionOf<TVector>()) + 
            "D";
    }

    static Unique<DataType>
    Load(
        Model& model,
        const Json& source)
    {
        const auto poles = source.at("Poles");
        const auto knotsU = source.at("KnotsU");
        const auto knotsV = source.at("KnotsV");
        const auto weights = source.value("Weights", std::vector<double>());
        
        const int degreeU = source.at("DegreeU");
        const int degreeV = source.at("DegreeV");
        const int nbPolesU = source.at("NbPolesU");
        const int nbPolesV = source.at("NbPolesV");
        const bool isRational = !weights.empty();

        auto result = New<DataType>(degreeU, degreeV, nbPolesU, nbPolesV,
            isRational);

        for (int i = 0; i < knotsU.size(); i++) {
            result->SetKnotU(i, knotsU[i]);
        }

        for (int i = 0; i < knotsV.size(); i++) {
            result->SetKnotV(i, knotsV[i]);
        }

        for (int i = 0; i < poles.size(); i++) {
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
        target["DegreeU"] = data.DegreeU();
        target["DegreeV"] = data.DegreeV();
        target["KnotsU"] = data.KnotsU();
        target["KnotsV"] = data.KnotsV();
        target["Poles"] = ToJson(data.Poles());

        if (data.IsRational()) {
            target["Weights"] = ToJson(data.Weights());
        }
    }
};

} // namespace ANurbs