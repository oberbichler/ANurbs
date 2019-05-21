#pragma once

#include "../../Model/Ref.h"
#include "../../Model/Model.h"
#include "BrepTrim.h"

#include "../../Model/Json.h"

#include <vector>

namespace ANurbs {

class BrepFace;

class BrepLoop
{
private:
    Ref<BrepFace> m_face;
    std::vector<Ref<BrepTrim>> m_trims;

public:
    static std::string
    Type()
    {
        return "BrepLoop";
    }

    Ref<BrepFace>
    Face()
    {
        return m_face;
    }

    size_t
    NbTrims()
    {
        return m_trims.size();
    }

    Ref<BrepTrim>
    Trim(
        size_t index)
    {
        return m_trims[index];
    }

    std::vector<Ref<BrepTrim>>
    Trims()
    {
        return m_trims;
    }

    static Unique<BrepLoop>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<BrepLoop>();

        // Read Face
        {
            const std::string key = data.at("Face");
            result->m_face = model.GetLazy<BrepFace>(key);
        }

        // Read Trims
        {
            const auto trims = data.at("Trims");

            result->m_trims.resize(trims.size());

            for (size_t i = 0; i < trims.size(); i++) {
                const std::string key = trims[i];
                result->m_trims[i] = model.GetLazy<BrepTrim>(key);
            }
        }

        return result;
    }

    void
    Save(
        const Model& model,
        Json& data) const
    {
        data["Face"] = ToJson(m_face);
        data["Trims"] = ToJson(m_trims);
    }

public:     // serialization
    static std::string type_name()
    {
        return "BrepLoop";
    }
};

} // namespace ANurbs