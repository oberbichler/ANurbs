#pragma once

#include <set>
#include <unordered_map>

namespace ANurbs {

class Attributes
{
private:
    std::set<std::string> m_tags;
    std::unordered_map<std::string, std::string> m_userStrings;

public:
    template <typename TModel, typename TSource>
    void
    Load(
        TModel& model,
        const TSource& source)
    {
        m_tags = source.value("Tags", std::set<std::string>());
        m_userStrings = source.value("UserStrings",
            std::unordered_map<std::string, std::string>());
    }

    template <typename TModel, typename TTarget>
    void
    Save(
        const TModel& model,
        TTarget& target) const
    {
        if (m_tags.size() != 0) {
            target["Tags"] = m_tags;
        }

        if (m_userStrings.size() != 0) {
            target["UserStrings"] = m_userStrings;
        }
    }

public:
    bool
    HasTag(
        const std::string& name)
    {
        return (m_tags.find(name) != m_tags.end());
    }

    void
    AddTag(
        const std::string& name)
    {
        m_tags.insert(name);
    }

    void
    RemoveTag(
        const std::string& name)
    {
        m_tags.erase(name);
    }

    bool
    HasUserString(
        const std::string& name)
    {
        return (m_userStrings.find(name) != m_userStrings.end());
    }

    std::string
    UserString(
        const std::string& name)
    {
        return m_userStrings.at(name);
    }

    void
    SetUserString(
        const std::string& name,
        const std::string& value)
    {
        m_userStrings.insert({name, value});
    }

    void
    RemoveUserString(
        const std::string& name)
    {
        m_userStrings.erase(name);
    }
};

} // namespace ANurbs