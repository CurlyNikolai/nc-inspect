#ifndef NETCDFREADER_H
#define NETCDFREADER_H

#include <filesystem>
#include <vector>
#include <limits>

#include "Node.h"
#include "netcdf.h"

class NetCDFReader
{
public:
    NetCDFReader(std::filesystem::path file_path);
    ~NetCDFReader();

    void open();
    void close();

    std::vector<int32_t> get_group_ids(const int32_t parent_group_id) const;
    std::string get_group_name(const int32_t group_id) const;
    std::vector<std::string> get_group_names(const int32_t parent_group_id) const;
    std::vector<std::string> get_group_names(const std::vector<int32_t>& group_ids) const;

    std::vector<int32_t> get_variable_ids(const int32_t parent_group_id) const;
    std::vector<std::string> get_variable_names(const int32_t parent_group_id) const;
    std::vector<std::string> get_variable_names(const int32_t parent_group_id, const std::vector<int32_t>& variable_ids) const;
    std::string get_variable_name(const int32_t parent_group_id, const int32_t variable_id) const;
    std::string get_variable_path(const int32_t parent_group_id, const int32_t variable_id) const;

    std::vector<int32_t> get_variable_dimension_ids(const int32_t parent_group_id, const int32_t variable_id) const;
    std::vector<size_t> get_variable_dimension_lengths(const int32_t parent_group_id, const int32_t variable_id) const;
    std::vector<std::string> get_variable_dimension_names(const int32_t parent_group_id, const int32_t variable_id) const;
    size_t get_variable_number_of_dimensions(const int32_t parent_group_id, const int32_t variable_id) const;

    size_t get_dimension_length(const int32_t parent_group_id, const int32_t dimension_id) const;
    std::string get_dimension_name(const int32_t parent_group_id, const int32_t dimension_id) const;

    std::string get_type(const int32_t parent_group_id, const int32_t variable_id) const;
    
    double get_fill_value(const int32_t parent_group_id, const int32_t variable_id) const;

    void read_into_group_nodes(GroupNode& group);
    void read_into_group_nodes(GroupNode& group, int32_t group_id);

    void read_var_double_data(int32_t grp_id, int32_t var_id, const std::vector<size_t>& start_idx, const std::vector<size_t>& count, std::vector<double>& data, const bool set_nans_zero = false) 
    {
        data.clear();
        size_t n = 1;
        for (const auto c : count)
        {
            n *= c;
        }
        data.resize(n);
        std::fill(data.begin(), data.end(), static_cast<double>(0));

        int32_t status = nc_get_vara_double(grp_id, var_id, start_idx.data(), count.data(), data.data());
        if (status != NC_NOERR)
        {
            fmt::print("Error ({}) when reading nc data.\n", status);
            fmt::print("Error: {}.\n", nc_strerror(status));
        }

        if (set_nans_zero)
        {
            for (auto& val : data)
            {
                if (std::isnan(val))
                {
                    val = 0.0;
                }
            }
        }
    };

    void read_var_double_data(int32_t grp_id, int32_t var_id, const std::vector<int32_t>& start_idx, const std::vector<size_t>& count, std::vector<double>& data, const bool set_nans_zero = false) 
    {
        std::vector<size_t> start_indices_new(start_idx.begin(), start_idx.end());
        read_var_double_data(grp_id, var_id, start_indices_new, count, data, set_nans_zero); 
    };

    bool is_open {false};
    std::filesystem::path path;
    int32_t root_grp_id;
};

#endif // NETCDFREADER_H