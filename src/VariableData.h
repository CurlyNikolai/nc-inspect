#ifndef VARIABLEDATA_H
#define VARIABLEDATA_H

#include <vector>
#include <string>
#include <memory>

struct VariableData
{
    enum class DimensionType
    {
        None,
        Scalar,
        OneDimensional,
        MultiDimensional
    };

    std::string name;
    std::string path;
    std::string type;
    std::vector<size_t> dimenson_lengths;
    std::vector<std::string> dimension_names;
    DimensionType dimension_type;

    void update_dimension_type();
};

#endif // VARIABLEDATA_H