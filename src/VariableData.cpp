#include "VariableData.h"

void VariableData::update_dimension_type()
{
    size_t dims_larger_than_one{0};
    for (const auto& dim_length : dimenson_lengths)
    {
        if (dim_length > 1)
        {
            dims_larger_than_one++;
        }
    }

    if (dims_larger_than_one == 0)
    {
        dimension_type = DimensionType::Scalar;
    }
    else if (dims_larger_than_one == 1)
    {
        dimension_type = DimensionType::OneDimensional;
    }
    else if (dims_larger_than_one >= 2)
    {
        dimension_type = DimensionType::MultiDimensional;
    }
    else 
    {
        dimension_type = DimensionType::None;
    }
}