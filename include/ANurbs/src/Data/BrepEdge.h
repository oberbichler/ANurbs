#pragma once

#include "../../Model/Ref.h"
#include "../../Model/Model.h"

#include "../../Model/Json.h"

#include <vector>

namespace ANurbs {

class BrepTrim;

class BrepEdge
{
private:
    std::vector<Ref<BrepTrim>> m_trims;

public:
    static std::string
    Type()
    {
        return "BrepEdge";
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

    static Unique<BrepEdge>
    Load(
        Model& model,
        const Json& data)
    {
        auto result = New<BrepEdge>();

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
        data["Trims"] = ToJson(m_trims);
    }

public:     // serialization
    static std::string type_name()
    {
        return "BrepEdge";
    }
};

} // namespace ANurbs