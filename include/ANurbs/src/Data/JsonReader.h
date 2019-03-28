#pragma once

#include "Json.h"
#include "TypeRegistry.h"
#include "Model.h"

#include <fstream>
#include <stdexcept>

namespace ANurbs {

template <typename TModel>
class JsonReader
{
private:
    static bool
    Load(
        TModel& model,
        const Json& source)
    {
        const auto type = GetTypeFromJson(source);

        const auto success = TypeRegistry<TModel>::Load(type, model, source);

        if (!success) {
            throw std::runtime_error("Unknown data type \"" + type + "\"");
        }

        return success;
    }

public:
    template <typename TSource>
    static void
    LoadArray(
        TModel& model,
        TSource& source)
    {
        Json::parser_callback_t cb = [&](int depth, Json::parse_event_t event,
            Json& parsed) {
            if (depth == 1 && event == Json::parse_event_t::object_end) {
                Load(model, parsed);

                return false;
            }
            return true;
        };

        Json::parse(source, cb);
    }

    template <typename TSource>
    static void
    LoadObject(
        TModel& model,
        TSource& source)
    {
        const auto parsed = Json::parse(source);

        Load(model, parsed);
    }

    static void
    LoadFile(
        TModel& model,
        const std::string& path)
    {
        std::fstream file;

        file.open(path, std::fstream::in);

        LoadArray(model, file);
    }
};

} // namespace ANurbs