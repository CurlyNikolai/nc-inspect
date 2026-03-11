#ifndef PLOTDATA_H
#define PLOTDATA_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

struct PlotData
{
    enum class Type
    {
        None,
        Line,
        Scatter,
        Heatmap
    };

    Type type{Type::None};

    Type get_type_from_string(const std::string& type_str);
    void set_type(const std::string& type_str);
    std::vector<double> x_data;
    std::vector<double> y_data;
    std::vector<double> z_data;
    int32_t x_dim;
    int32_t y_dim;
    std::string x_name;
    std::string y_name;
    std::unordered_map<std::string, int32_t> coordinates;
    std::vector<int32_t> start_indices;
};

#endif // PLOTDATA_H