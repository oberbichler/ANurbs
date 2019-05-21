#pragma once

#include "Attributes.h"

namespace ANurbs {

class CadAttributes : public Attributes
{
private:
    std::string m_layer;
    std::string m_color;
    std::string m_arrowhead;

public:
    template <typename TModel, typename TSource>
    void
    Load(
        TModel& model,
        const TSource& source)
    {
        Attributes::Load(model, source);

        m_layer = source.value("Layer", "");
        m_color = source.value("Color", "");
        m_arrowhead = source.value("Arrowhead", "");
    }

    template <typename TModel, typename TTarget>
    void
    Save(
        const TModel& model,
        TTarget& target) const
    {
        Attributes::Save(model, target);

        if (!m_layer.empty()) {
            target["Layer"] = m_layer;
        }
        
        if (!m_color.empty()) {
            target["Color"] = m_color;
        }
        
        if (!m_arrowhead.empty()) {
            target["Arrowhead"] = m_arrowhead;
        }
    }

public:
    std::string
    Layer() const
    {
        return m_layer;
    }

    void
    SetLayer(
        const std::string& value)
    {
        m_layer = value;
    }

    std::string
    Color() const
    {
        return m_color;
    }

    void
    SetColor(
        const std::string& value)
    {
        m_color = value;
    }

    std::string
    Arrowhead() const
    {
        return m_arrowhead;
    }

    void
    SetArrowhead(
        const std::string& value)
    {
        m_arrowhead = value;
    }
};

} // namespace ANurbs