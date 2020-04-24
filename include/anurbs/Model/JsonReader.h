#pragma once

#include "../Define.h"

#include "Json.h"
#include "TypeRegistry.h"
#include "Model.h"

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#include <fstream>
#include <stdexcept>

namespace anurbs {

template <typename TModel>
class JsonReader
{
private:
    static bool load(TModel& model, const Json& source)
    {
        const auto type = type_name_from_json(source);

        const auto success = TypeRegistry<TModel>::load(type, model, source);

        if (!success) {
            throw std::runtime_error("Unknown data type \"" + type + "\"");
        }

        return success;
    }

public:
    template <typename TSource>
    static void load_array(TModel& model, TSource& source)
    {
        Json::parser_callback_t cb = [&](int depth, Json::parse_event_t event, Json& parsed) {
            if (depth == 1 && event == Json::parse_event_t::object_end) {
                load(model, parsed);

                return false;
            }
            return true;
        };

        auto no_warning = Json::parse(source, cb);
    }

    template <typename TSource>
    static void load_object(TModel& model, TSource& source)
    {
        const auto parsed = Json::parse(source);

        load(model, parsed);
    }

    static void load_file(TModel& model, const std::string& path)
    {
        if (!fs::exists(path)) {
            throw std::runtime_error("File not found");
        }

        std::fstream file;

        file.open(path, std::fstream::in);

        load_array(model, file);
    }
};

} // namespace anurbs