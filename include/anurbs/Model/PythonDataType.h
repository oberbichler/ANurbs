#pragma once

#include "../Define.h"

namespace anurbs {

template <typename TModel>
struct PythonDataTypeBase
{
    virtual ~PythonDataTypeBase() { }

    virtual pybind11::object get(const TModel& model, const std::string& key)
        = 0;

    virtual pybind11::object get(const TModel& model, const size_t key)
        = 0;

    virtual pybind11::object of_type(const TModel& model) = 0;

    static inline std::unordered_map<std::string, Unique<PythonDataTypeBase>>
        s_types;

    static pybind11::object by_key(const TModel& model,
        const std::string& key)
    {
        const auto type = model.get_type(key);
        return s_types[type]->get(model, key);
    }

    static pybind11::object by_index(const TModel& model,
        const size_t index)
    {
        const auto type = model.get_type(index);
        return s_types[type]->get(model, index);
    }

    static pybind11::object by_type(const TModel& model,
        const std::string& type)
    {
        return s_types[type]->of_type(model);
    }
};

template <typename TModel, typename TData>
struct PythonDataType : public PythonDataTypeBase<TModel>
{
    pybind11::object get(const TModel& model, const std::string& key)
        override
    {
        return pybind11::cast(model.template get<TData>(key));
    }

    pybind11::object get(const TModel& model, const size_t index)
        override
    {
        return pybind11::cast(model.template get<TData>(index));
    }

    pybind11::object of_type(const TModel& model) override
    {
        return pybind11::cast(model.template of_type<TData>());
    }

    static Ref<TData> add(TModel& self, Pointer<TData> data)
    {
        return self.template add<TData>(data);
    }

    static Ref<TData> add_with_attributes(TModel& self, Pointer<TData> data,
        const std::string& attributes)
    {
        return self.template add<TData>(data, attributes);
    }

    static Ref<TData> add_with_key(TModel& self, const std::string& key,
        Pointer<TData> data)
    {
        return self.template add<TData>(key, data);
    }

    static Ref<TData> replace(TModel& self, const size_t index,
        Pointer<TData> data)
    {
        return self.template replace<TData>(index, data);
    }

    static Ref<TData> replace_with_key(TModel& self, const std::string& key,
        Pointer<TData> data)
    {
        return self.template replace<TData>(key, data);
    }
};

} // namespace anurbs