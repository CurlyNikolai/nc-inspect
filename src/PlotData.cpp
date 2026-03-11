#include "PlotData.h"

PlotData::Type PlotData::get_type_from_string(const std::string &type_str)
{
    if (type_str == "Line")
    {
        return Type::Line;
    }
    else if (type_str == "Scatter")
    {
        return Type::Scatter;
    }
    else if (type_str == "Heatmap")
    {
        return Type::Heatmap;
    }
    return Type::None;
}

void PlotData::set_type(const std::string &type_str)
{
    type = get_type_from_string(type_str);
}
