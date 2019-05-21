#pragma once

#include "Define.h"

#include "Json.h"
#include "TypeRegistry.h"
#include "Model.h"

#include <stdexcept>

namespace ANurbs {

template <typename TModel>
struct JsonWriter
{
    template <typename TStream>
    static void save_stream(const TModel& model, TStream& stream)
    {
        stream << "[" << std::endl;

        size_t write_count = 0;

        for (size_t i = 0; i < model.NbEntries(); i++) {
            const auto type = model.GetType(i);
            const auto key = model.GetKey(i);

            Json data;

            if (!key.empty()) {
                data["Key"] = key;
            }
            data["Type"] = type;

            if (!TypeRegistry<TModel>::save(type, model, i, data)) {
                continue;
            }

            if (write_count == 0) {
                stream << "  {\n";
            } else {
                stream << ",\n  {\n";
            }

            auto it = data.begin();
            if (it != data.end()) {
                stream << "    \"" << it.key() << "\": " << it->dump();
            }
            for (++it; it != data.end(); ++it) {
                stream << ",\n    \"" << it.key() << "\": " << it->dump();
            }

            stream << "\n  }";

            write_count += 1;
        }

        stream << "\n]" << std::endl;
    }

    static void save_file(const TModel& model, const std::string& path)
    {
        std::fstream file;

        file.open(path, std::fstream::out);

        save_stream(model, file);
    }
};

} // namespace ANurbs