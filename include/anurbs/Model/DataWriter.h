#pragma once

#include "Json.h"

namespace anurbs {

class DataWriter
{
private: // variables
    Json& m_data;

public: // constructors
    DataWriter(Json& data) : m_data(data)
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
    void write(const std::string& key, const T& value)
    {
        m_data[key] = value;
    }

    template <typename T>
    void write_vector(const std::string& key, const T& vector)
    {
        Json data;

        for (Index i = 0; i < length(vector); i++) {
            data.push_back(vector[i]);
        }

        m_data[key] = data;
    }

    template <typename T>
    void write_matrix(const std::string& key, T matrix)
    {
        Json data;

        for (Index i = 0; i < matrix.rows(); i++) {
            Json row_data;
            
            for (Index j = 0; j < matrix.cols(); j++) {
                row_data.push_back(matrix(i, j));
            }
            
            data.push_back(row_data);
        }

        m_data[key] = data;
    }
};

} // namespace anurbs