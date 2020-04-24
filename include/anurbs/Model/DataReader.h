#pragma once

#include "Json.h"

namespace anurbs {

class DataReader
{
private: // variables
    const Json& m_data;

public: // constructors
    DataReader(const Json& data) : m_data(data)
    {
    }

public: // methods
    bool has(const std::string& key) const
    {
        return m_data.find(key) != m_data.end();
    }

    Index count(const std::string& key) const
    {
        return length(m_data.at(key));
    }

    template <typename T>
    T read(const std::string& key) const
    {
        return m_data.at(key);
    }

    template <typename T>
    T read(const std::string& key, const T default_value) const
    {
        const auto it = m_data.find(key);

        if (it == m_data.end()) {
            return default_value;
        }

        return it;
    }

    template <typename T>
    void fill_vector(const std::string& key, T vector) const
    {
        const auto data = m_data.at(key);
        
        assert(data.size() == vector.size());

        for (Index i = 0; i < length(data); i++) {
            vector[i] = data.at(i);
        }
    }

    template <typename T>
    void fill_matrix(const std::string& key, T matrix) const
    {
        const auto data = m_data.at(key);
        
        assert(data.size() == matrix.rows());

        for (Index i = 0; i < length(data); i++) {
            const auto row_data = data.at(i);

            assert(row_data.size() == matrix.cols());

            for (Index j = 0; j < length(row_data); j++) {
                matrix(i, j) = row_data.at(j);
            }
        }
    }
};

} // namespace anurbs