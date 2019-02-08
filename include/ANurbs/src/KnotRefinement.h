#pragma once

#include "CurveGeometry.h"
#include "Knots.h"
#include "SurfaceGeometry.h"

#include <algorithm>
#include <vector>

namespace ANurbs {

class KnotRefinement
{

public:
    template <typename TCurveGeometry>
    static Pointer<CurveGeometry<typename TCurveGeometry::VectorType>>
    InsertKnots(
        const TCurveGeometry& surfaceGeometry,
        std::vector<typename TCurveGeometry::ScalarType> knots)
    {
        using CurveGeometryType = CurveGeometry<TCurveGeometry::VectorType>;

        std::sort(knots.begin(), knots.end());

        const int nbKnotsToInsert = static_cast<int>(knots.size());

        const int degree = surfaceGeometry.Degree();

        const int nbPoles = surfaceGeometry.NbPoles();

        const int nbKnots = surfaceGeometry.NbKnots();

        const int a = Knots::UpperSpan(degree, surfaceGeometry.Knots(),
            knots.front());
        const int b = Knots::UpperSpan(degree, surfaceGeometry.Knots(),
            knots.back());

        const int nbPolesRefined = nbPoles + nbKnotsToInsert;
        const int nbKnotsRefined = nbKnots + nbKnotsToInsert + 2;

        Pointer<CurveGeometryType> refined = New<CurveGeometryType>(degree,
            nbPolesRefined, true); // FIXME: isRational

        for (int i = 0; i < a + 1 - degree + 1; i++) {
            refined->SetPole(i, surfaceGeometry.Pole(i) * surfaceGeometry.Weight(i));
            refined->SetWeight(i, surfaceGeometry.Weight(i));
        }

        for (int i = b + 2 - 1; i < nbPoles; i++) {
            refined->SetPole(nbKnotsToInsert + i, surfaceGeometry.Pole(i) * surfaceGeometry.Weight(i));
            refined->SetWeight(nbKnotsToInsert + i, surfaceGeometry.Weight(i));
        }

        for (int i = 0; i < a + 1 + 1 - 1; i++) {
            refined->SetKnot(i, surfaceGeometry.Knot(i));
        }
        
        for (int i = b + 2 + degree - 1; i < nbPoles + degree + 1 - 2 - 1; i++) {
            refined->SetKnot(i + nbKnotsToInsert, surfaceGeometry.Knot(i));
        }

        const int n = nbPoles - 1;
        const int m = n + degree + 1;
        const int r = nbKnotsToInsert - 1;

        int i = b + 2 + degree - 1;
        int k = b + 2 + degree + r;
        int j = r;

        while (j >= 0) {
            while (knots[j] <= surfaceGeometry.Knot(-1 + i) && i > a + 1) {
                const auto pole = surfaceGeometry.Pole(i - degree - 1);
                const auto weight = surfaceGeometry.Weight(i - degree - 1);
                refined->SetPole(k - degree - 1, pole * weight);
                refined->SetWeight(k - degree - 1, weight);

                refined->SetKnot(-1+k, surfaceGeometry.Knot(-1 + i));

                k -= 1;
                i -= 1;
            }

            refined->SetPole(k - degree - 1, refined->Pole(k - degree));
            refined->SetWeight(k - degree - 1, refined->Weight(k - degree));

            for (int l = 1; l < degree + 1; l++) {
                const int index = k - degree + l;
                auto alpha = refined->Knot(-1+k + l) - knots[j];

                if (std::abs(alpha) < 1e-7) {
                    refined->SetPole(index - 1, refined->Pole(index));
                    refined->SetWeight(index - 1, refined->Weight(index));
                } else {
                    alpha = alpha / (refined->Knot(k + l - 1) - surfaceGeometry.Knot(i + l - degree - 1));
                    refined->SetPole(index - 1, refined->Pole(index - 1) * alpha + refined->Pole(index) * (1 - alpha));
                    refined->SetWeight(index - 1, refined->Weight(index - 1) * alpha + refined->Weight(index) * (1 - alpha));
                }
            }

            refined->SetKnot(-1 + k, knots[j]);

            k -= 1;
            j -= 1;
        }

        for (int i = 0; i < refined->NbPoles(); i++) {
            refined->SetPole(i, refined->Pole(i) / refined->Weight(i));
        }

        return refined;                
    }

    template <typename TSurfaceGeometry>
    static Pointer<SurfaceGeometry<typename TSurfaceGeometry::VectorType>>
    InsertKnotsU(
        const TSurfaceGeometry& surfaceGeometry,
        std::vector<typename TSurfaceGeometry::ScalarType> knotsU)
    {
        using SurfaceGeometryType = SurfaceGeometry<
            TSurfaceGeometry::VectorType>;

        std::sort(knotsU.begin(), knotsU.end());

        const int nbKnotsToInsert = static_cast<int>(knotsU.size());

        const int degreeU = surfaceGeometry.DegreeU();
        const int degreeV = surfaceGeometry.DegreeV();

        const int nbPolesU = surfaceGeometry.NbPolesU();
        const int nbPolesV = surfaceGeometry.NbPolesV();

        const int nbKnotsU = surfaceGeometry.NbKnotsU();
        const int nbKnotsV = surfaceGeometry.NbKnotsV();

        const int a = Knots::UpperSpan(degreeU, surfaceGeometry.KnotsU(),
            knotsU.front());
        const int b = Knots::UpperSpan(degreeU, surfaceGeometry.KnotsU(),
            knotsU.back());

        const int nbPolesRefined = nbPolesU + nbKnotsToInsert;
        const int nbKnotsRefined = surfaceGeometry.NbKnotsU() + 2 + nbKnotsToInsert;

        Pointer<SurfaceGeometryType> refined = New<SurfaceGeometryType>(degreeU,
            degreeV, nbPolesRefined, nbPolesV, true); // FIXME: isRational

        for (int i = 0; i < a + 1 - degreeU + 1; i++) {
            for (int m = 0; m < surfaceGeometry.NbPolesV(); m++) {
                refined->SetPole(i, m, surfaceGeometry.Pole(i, m) * surfaceGeometry.Weight(i, m));
                refined->SetWeight(i, m, surfaceGeometry.Weight(i, m));
            }
        }

        for (int i = b + 2 - 1; i < nbPolesU; i++) {
            for (int m = 0; m < surfaceGeometry.NbPolesV(); m++) {
                refined->SetPole(nbKnotsToInsert + i, m, surfaceGeometry.Pole(i, m) * surfaceGeometry.Weight(i, m));
                refined->SetWeight(nbKnotsToInsert + i, m, surfaceGeometry.Weight(i, m));
            }
        }

        for (int i = 0; i < a + 1 + 1 - 1; i++) {
            refined->SetKnotU(i, surfaceGeometry.KnotU(i));
        }
        
        for (int i = b + 2 + degreeU - 1; i < nbPolesU + degreeU + 1 - 2 - 1; i++) {
            refined->SetKnotU(i + nbKnotsToInsert, surfaceGeometry.KnotU(i));
        }

        for (int i = 0; i < surfaceGeometry.NbKnotsV(); i++) {
            refined->SetKnotV(i, surfaceGeometry.KnotV(i));
        }

        const int n = nbPolesU - 1;
        const int m = n + degreeU + 1;
        const int r = nbKnotsToInsert - 1;

        int i = b + 2 + degreeU - 1;
        int k = b + 2 + degreeU + r;
        int j = r;

        while (j >= 0) {
            while (knotsU[j] <= surfaceGeometry.KnotU(-1 + i) && i > a + 1) {
                for (int m = 0; m < surfaceGeometry.NbPolesV(); m++) {
                    const auto pole = surfaceGeometry.Pole(i - degreeU - 1, m);
                    const auto weight = surfaceGeometry.Weight(i - degreeU - 1, m);
                    refined->SetPole(k - degreeU-1, m, pole * weight);
                    refined->SetWeight(k - degreeU-1, m, weight);
                }

                refined->SetKnotU(-1+k, surfaceGeometry.KnotU(-1 + i));

                k -= 1;
                i -= 1;
            }

            for (int m = 0; m < surfaceGeometry.NbPolesV(); m++) {
                refined->SetPole(k - degreeU-1, m, refined->Pole(k - degreeU, m));
                refined->SetWeight(k - degreeU-1, m, refined->Weight(k - degreeU, m));
            }

            for (int l = 1; l < degreeU + 1; l++) {
                const int index = k - degreeU + l;
                auto alpha = refined->KnotU(-1+k + l) - knotsU[j];

                if (std::abs(alpha) < 1e-7) {
                    for (int m = 0; m < surfaceGeometry.NbPolesV(); m++) {
                        refined->SetPole(index - 1, m, refined->Pole(index, m));
                        refined->SetWeight(index - 1, m, refined->Weight(index, m));
                    }
                } else {
                    alpha = alpha / (refined->KnotU(k + l - 1) - surfaceGeometry.KnotU(i + l - degreeU - 1));
                    for (int m = 0; m < surfaceGeometry.NbPolesV(); m++) {
                        refined->SetPole(index - 1, m, refined->Pole(index - 1, m) * alpha + refined->Pole(index, m) * (1 - alpha));
                        refined->SetWeight(index - 1, m, refined->Weight(index - 1, m) * alpha + refined->Weight(index, m) * (1 - alpha));
                    }
                }
            }

            refined->SetKnotU(-1 + k, knotsU[j]);

            k -= 1;
            j -= 1;
        }

        for (int i = 0; i < refined->NbPoles(); i++) {
            refined->SetPole(i, refined->Pole(i) / refined->Weight(i));
        }

        return refined;                
    }
    
    template <typename TSurfaceGeometry>
    static Pointer<SurfaceGeometry<typename TSurfaceGeometry::VectorType>>
    InsertKnotsV(
        const TSurfaceGeometry& surfaceGeometry,
        std::vector<typename TSurfaceGeometry::ScalarType> knotsV)
    {
        using SurfaceGeometryType = SurfaceGeometry<
            TSurfaceGeometry::VectorType>;

        std::sort(knotsV.begin(), knotsV.end());

        const int nbKnotsToInsert = static_cast<int>(knotsV.size());

        const int degreeU = surfaceGeometry.DegreeU();
        const int degreeV = surfaceGeometry.DegreeV();

        const int nbPolesU = surfaceGeometry.NbPolesU();
        const int nbPolesV = surfaceGeometry.NbPolesV();

        const int nbKnotsU = surfaceGeometry.NbKnotsU();
        const int nbKnotsV = surfaceGeometry.NbKnotsV();

        const int a = Knots::UpperSpan(degreeV, surfaceGeometry.KnotsV(),
            knotsV.front());
        const int b = Knots::UpperSpan(degreeV, surfaceGeometry.KnotsV(),
            knotsV.back());

        const int nbPolesRefined = nbPolesV + nbKnotsToInsert;
        const int nbKnotsRefined = nbKnotsV + 2 + nbKnotsToInsert;

        Pointer<SurfaceGeometryType> refined = New<SurfaceGeometryType>(degreeU,
            degreeV, nbPolesU, nbPolesRefined, true); // FIXME: isRational

        for (int i = 0; i < a + 1 - degreeV + 1; i++) {
            for (int m = 0; m < surfaceGeometry.NbPolesU(); m++) {
                refined->SetPole(i, m, surfaceGeometry.Pole(i, m) * surfaceGeometry.Weight(i, m));
                refined->SetWeight(i, m, surfaceGeometry.Weight(i, m));
            }
        }

        for (int i = b + 2 - 1; i < nbPolesV; i++) {
            for (int m = 0; m < surfaceGeometry.NbPolesU(); m++) {
                refined->SetPole(m, nbKnotsToInsert + i, surfaceGeometry.Pole(m, i) * surfaceGeometry.Weight(m, i));
                refined->SetWeight(m, nbKnotsToInsert + i, surfaceGeometry.Weight(m, i));
            }
        }

        for (int i = 0; i < a + 1 + 1 - 1; i++) {
            refined->SetKnotV(i, surfaceGeometry.KnotV(i));
        }
        
        for (int i = b + 2 + degreeV - 1; i < nbPolesV + degreeV + 1 - 2 - 1; i++) {
            refined->SetKnotV(i + nbKnotsToInsert, surfaceGeometry.KnotV(i));
        }

        for (int i = 0; i < surfaceGeometry.NbKnotsU(); i++) {
            refined->SetKnotU(i, surfaceGeometry.KnotU(i));
        }

        const int n = nbPolesV - 1;
        const int m = n + degreeV + 1;
        const int r = nbKnotsToInsert - 1;

        int i = b + 2 + degreeV - 1;
        int k = b + 2 + degreeV + r;
        int j = r;

        while (j >= 0) {
            while (knotsV[j] <= surfaceGeometry.KnotV(-1 + i) && i > a + 1) {
                for (int m = 0; m < surfaceGeometry.NbPolesU(); m++) {
                    const auto pole = surfaceGeometry.Pole(m, i - degreeV - 1);
                    const auto weight = surfaceGeometry.Weight(m, i - degreeV - 1);
                    refined->SetPole(m, k - degreeV-1, pole * weight);
                    refined->SetWeight(m, k - degreeV-1, weight);
                }

                refined->SetKnotV(-1+k, surfaceGeometry.KnotV(-1 + i));

                k -= 1;
                i -= 1;
            }

            for (int m = 0; m < surfaceGeometry.NbPolesU(); m++) {
                refined->SetPole(m, k - degreeV-1, refined->Pole(m, k - degreeV));
                refined->SetWeight(m, k - degreeV-1, refined->Weight(m, k - degreeV));
            }

            for (int l = 1; l < degreeV + 1; l++) {
                const int index = k - degreeV + l;
                auto alpha = refined->KnotV(-1+k + l) - knotsV[j];

                if (std::abs(alpha) < 1e-7) {
                    for (int m = 0; m < surfaceGeometry.NbPolesU(); m++) {
                        refined->SetPole(m, index - 1, refined->Pole(m, index));
                        refined->SetWeight(m, index - 1, refined->Weight(m, index));
                    }
                } else {
                    alpha = alpha / (refined->KnotV(k + l - 1) - surfaceGeometry.KnotV(i + l - degreeV - 1));
                    for (int m = 0; m < surfaceGeometry.NbPolesU(); m++) {
                        refined->SetPole(m, index - 1, refined->Pole(m, index - 1) * alpha + refined->Pole(m, index) * (1 - alpha));
                        refined->SetWeight(m, index - 1, refined->Weight(m, index - 1) * alpha + refined->Weight(m, index) * (1 - alpha));
                    }
                }
            }

            refined->SetKnotV(-1 + k, knotsV[j]);

            k -= 1;
            j -= 1;
        }

        for (int i = 0; i < refined->NbPoles(); i++) {
            refined->SetPole(i, refined->Pole(i) / refined->Weight(i));
        }

        return refined;                
    }
};

}