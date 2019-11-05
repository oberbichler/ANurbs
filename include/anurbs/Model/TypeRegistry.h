#pragma once

#include "../Define.h"

#include "Json.h"

#include <map>
#include <stdexcept>

namespace anurbs {

template <typename TModel>
struct TypeEntryBase
{
    virtual ~TypeEntryBase() { }

    virtual bool load(TModel& model, const Json& source) = 0;

    virtual bool save(const TModel& model, const size_t index, Json& target)
        = 0;
};

template <typename TData, typename TModel>
struct TypeEntry : public TypeEntryBase<TModel>
{
    bool load(TModel& model, const Json& source) override
    {
        const auto key = key_from_json(source);

        Pointer<TData> data = TData::load(model, source);

        Ref<TData> ref;

        if (key.empty()) {
            ref = model.template add<TData>(data);
        } else {
            ref = model.template add<TData>(key, data);
        }

        ref.attributes()->load(model, source);

        return true;
    };

    bool save(const TModel& model, const size_t index, Json& target) override
    {
        const auto entry = model.template get<TData>(index);

        if (entry.is_empty() || entry.data() == nullptr) {
            return false;
        }

        TData::save(model, *entry, target);
        entry.attributes()->save(model, target);

        return true;
    };
};

template <typename TModel>
struct TypeRegistry
{
    static std::map<std::string, Unique<TypeEntryBase<TModel>>> s_registry;

    template <typename TData>
    static void register_type(bool no_exception)
    {
        const std::string type = TData::type_name();

        if (s_registry.find(type) != s_registry.end()) {
            if (no_exception) {
                return;
            }
            throw std::runtime_error("Entity already registered");
        }

        s_registry[type] = new_<TypeEntry<TData, TModel>>();
    }

    static bool load(const std::string type, TModel& model, const Json& source)
    {
        const auto it = s_registry.find(type);

        if (it != s_registry.end()) {
            it->second->load(model, source);
            return true;
        } else {
            return false;
        }
    }

    static bool save(const std::string type, const TModel& model,
        const size_t index, Json& target)
    {
        const auto it = s_registry.find(type);

        if (it != s_registry.end()) {
            return it->second->save(model, index, target);
        }

        return false;
    }
};

template <typename TModel>
std::map<std::string, Unique<TypeEntryBase<TModel>>>
    TypeRegistry<TModel>::s_registry;

} // namespace anurbs