#pragma once

#include "../../EigenSupport"
#include "Define.h"
#include "Ref.h"
#include "Model.h"
#include "BrepLoop.h"

#include "Json.h"

#include <vector>

namespace ANurbs {

class BrepFaceField
{
private:
    Ref<BrepFace> m_face;
    std::vector<Eigen::VectorXd> m_values;
    int m_dimension;

public:
    BrepFaceField()
    {
        m_dimension = 1;
    }

    BrepFaceField(int dimension)
    {
        m_dimension = dimension;
    }

    static std::string
    Type()
    {
        return "BrepFaceField";
    }

    Ref<BrepFace>
    Face()
    {
        return m_face;
    }

    void
    SetFace(Ref<BrepFace> value)
    {
        m_face = value;
        m_values.resize(value->Geometry()->NbPoles());
        for (int i = 0; i < m_values.size(); i++) {
            m_values[i] = Eigen::VectorXd::Zero(m_dimension);
        }
    }

    std::vector<Eigen::VectorXd>
    Values()
    {
        return m_values;
    }

    void
    SetValue(int index, const Eigen::VectorXd& value)
    {
        m_values[index] = value;
    }

    static Unique<BrepFaceField>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<BrepFaceField>();

        // Read Face
        {
            const std::string key = data.at("Face");
            result->m_face = model.GetLazy<BrepFace>(key);
        }

        // Read Dimension
        {
            result->m_dimension = data.at("Dimension");
        }

        // Read Values
        {
            const auto values = data.at("Values");

            result->m_values.resize(values.size());

            for (size_t i = 0; i < values.size(); i++) {
                result->m_values[i] = values[i];
            }
        }

        return result;
    }

    void
    Save(
        const Model& model,
        Json& data) const
    {
        data["Dimension"] = ToJson(m_dimension);
        data["Face"] = ToJson(m_face);
        data["Values"] = ToJson(m_values);
    }
};

} // namespace ANurbs