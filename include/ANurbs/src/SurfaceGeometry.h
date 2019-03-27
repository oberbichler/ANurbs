#pragma once

#include "Grid.h"
#include "Point.h"
#include "Pointer.h"
#include "SurfaceGeometryBase.h"

#include <stdexcept>
#include <vector>

namespace ANurbs {

template <typename TVector>
class SurfaceGeometry
    : public SurfaceGeometryBase<TVector>
{
public:
    using SurfaceGeometryType = SurfaceGeometry<TVector>;
    using SurfaceGeometryBaseType = SurfaceGeometryBase<TVector>;
    using typename SurfaceGeometryBaseType::KnotsType;
    using typename SurfaceGeometryBaseType::ScalarType;
    using typename SurfaceGeometryBaseType::VectorType;

protected:
    Grid<VectorType> m_poles;
    Grid<ScalarType> m_weights;

public:
    SurfaceGeometry(
        const int degreeU,
        const int degreeV,
        const int nbPolesU,
        const int nbPolesV,
        const bool isRational)
        : SurfaceGeometryBaseType(degreeU, degreeV, nbPolesU, nbPolesV)
        , m_poles(nbPolesU, nbPolesV)
        , m_weights(isRational ? nbPolesU : 0, isRational ? nbPolesV : 0)
    {
    }

    VectorType
    Pole(
        const int indexU,
        const int indexV) const override
    {
        return m_poles(indexU, indexV);
    }

    using SurfaceGeometryBase<VectorType>::Pole;

    void
    SetPole(
        const int indexU,
        const int indexV,
        const VectorType& value) override
    {
        return m_poles.SetValue(indexU, indexV, value);
    }

    using SurfaceGeometryBase<VectorType>::SetPole;

    ScalarType
    Weight(
        const int indexU,
        const int indexV) const override
    {
        if (IsRational()) {
            return m_weights(indexU, indexV);
        } else {
            return 1;
        }
    }

    using SurfaceGeometryBase<VectorType>::Weight;

    void
    SetWeight(
        const int indexU,
        const int indexV,
        const ScalarType value) override
    {
        if (IsRational()) {
            return m_weights.SetValue(indexU, indexV, value);
        } else {
            throw std::invalid_argument("Geometry is not rational");
        }
    }

    using SurfaceGeometryBase<VectorType>::SetWeight;

    bool
    IsRational() const override
    {
        return m_weights.NbValues() != 0;
    }

    Pointer<SurfaceGeometryType>
    Clone()
    {
        Pointer<SurfaceGeometryType> clone = New<SurfaceGeometryType>(
            this->DegreeU(), this->DegreeV(), this->NbPolesU(),
            this->NbPolesV(), this->IsRational());

        for (int i = 0; i < this->NbKnotsU(); i++) {
            clone->SetKnotU(i, this->KnotU(i));
        }

        for (int i = 0; i < this->NbKnotsV(); i++) {
            clone->SetKnotV(i, this->KnotV(i));
        }

        for (int i = 0; i < this->NbPoles(); i++) {
            clone->SetPole(i, this->Pole(i));
        }

        if (this->IsRational()) {
            for (int i = 0; i < this->NbPoles(); i++) {
                clone->SetWeight(i, this->Weight(i));
            }
        }

        return clone;
    }
};

using SurfaceGeometry1D = SurfaceGeometry<Point1D>;
using SurfaceGeometry2D = SurfaceGeometry<Point2D>;
using SurfaceGeometry3D = SurfaceGeometry<Point3D>;

} // namespace ANurbs
