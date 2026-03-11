#include <fmt/core.h>
#include <fmt/ranges.h>
#include <array>

#include "NetCDFReader.h"

NetCDFReader::NetCDFReader(std::filesystem::path file_path) : path(file_path)
{
}

NetCDFReader::~NetCDFReader()
{
    if (is_open)
    {
        close();
    }
}

void NetCDFReader::open()
{
    if (is_open)
    {
        fmt::print("File {} already open.\n", path.c_str());
        return;
    }

    int32_t status = static_cast<int32_t>(nc_open(path.c_str(), NC_NOWRITE, &root_grp_id));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to open file {} (status code {}).\n", path.c_str(), status);
        return;
    }

    is_open = true;
}

void NetCDFReader::close()
{
    int32_t status = static_cast<int32_t>(nc_close(root_grp_id));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to close file {} (status code: {}).\n", path.c_str(), status);
        return;
    }

    is_open = false;
}

std::vector<int32_t> NetCDFReader::get_group_ids(int32_t parent_group_id) const
{
    int32_t n_groups;
    int32_t status1 = static_cast<int32_t>(nc_inq_grps(parent_group_id, &n_groups, nullptr));
    if (status1 != NC_NOERR)
    {
        fmt::print("Unable to read group ID:s for group {} (status code: {}).\n", parent_group_id, status1);
        return {};
    }

    std::vector<int32_t> group_ids(n_groups);
    int32_t status2 = static_cast<int32_t>(nc_inq_grps(parent_group_id, &n_groups, group_ids.data()));
    if (status2 != NC_NOERR)
    {
        fmt::print("Unable to read group ID:s for group {} (status code: {}).\n", parent_group_id, status2);
        return {};
    }

    return group_ids;
}

std::string NetCDFReader::get_group_name(const int32_t group_id) const
{
    std::array<char, NC_MAX_NAME + 1> c_str;

    int32_t status = static_cast<int32_t>(nc_inq_grpname(group_id, c_str.data()));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to read group name for ID {}.\n", group_id);
        return "NoName";
    }

    return c_str.data();
}

std::vector<std::string> NetCDFReader::get_group_names(int32_t parent_group_id) const
{
    return get_group_names(get_group_ids(parent_group_id));
}

std::vector<std::string> NetCDFReader::get_group_names(const std::vector<int32_t> &group_ids) const
{
    std::vector<std::string> group_names;

    for (const auto &group_id : group_ids)
    {
        group_names.push_back(get_group_name(group_id));
    }

    return group_names;
}

std::vector<int32_t> NetCDFReader::get_variable_ids(const int32_t parent_group_id) const
{
    int32_t n_variables;
    int32_t status1 = static_cast<int32_t>(nc_inq_varids(parent_group_id, &n_variables, nullptr));
    if (status1 != NC_NOERR)
    {
        fmt::print("Unable to read variable ID:s for group {} (status code: {}).\n", parent_group_id, status1);
        return {};
    }

    std::vector<int32_t> variable_ids(n_variables);
    int32_t status2 = static_cast<int32_t>(nc_inq_varids(parent_group_id, &n_variables, variable_ids.data()));
    if (status2 != NC_NOERR)
    {
        fmt::print("Unable to read variable ID:s for group {} (status code: {}).\n", parent_group_id, status2);
        return {};
    }

    return variable_ids;
}

std::vector<std::string> NetCDFReader::get_variable_names(const int32_t parent_group_id) const
{
    return get_variable_names(parent_group_id, get_variable_ids(parent_group_id));
}

std::vector<std::string> NetCDFReader::get_variable_names(const int32_t parent_group_id, const std::vector<int32_t> &variable_ids) const
{
    std::vector<std::string> variable_names;

    for (const auto &variable_id : variable_ids)
    {
        variable_names.push_back(get_variable_name(parent_group_id, variable_id));
    }

    return variable_names;
}

std::string NetCDFReader::get_variable_name(const int32_t parent_group_id, const int32_t variable_id) const
{
    std::array<char, NC_MAX_NAME + 1> c_str;

    int32_t status = static_cast<int32_t>(nc_inq_varname(parent_group_id, variable_id, c_str.data()));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to find name for variable id {}.\n", variable_id);
        return "NO_NAME";
    }

    return c_str.data();
}

std::string NetCDFReader::get_variable_path(const int32_t parent_group_id, const int32_t variable_id) const
{
    const std::string var_name = get_variable_name(parent_group_id, variable_id);

    std::array<char, NC_MAX_NAME + 1> c_str;
    size_t l = 0;

    int32_t status = static_cast<int32_t>(nc_inq_grpname_full(parent_group_id, &l, c_str.data()));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to find group path for variable id {}.\n", variable_id);
        return "NO_NAME";
    }

    return fmt::format("{}/{}", c_str.data(), var_name);
}

std::vector<int32_t> NetCDFReader::get_variable_dimension_ids(const int32_t parent_group_id, const int32_t variable_id) const
{
    std::vector<int32_t> dim_ids(get_variable_number_of_dimensions(parent_group_id, variable_id));
    
    int32_t status = static_cast<int32_t>(nc_inq_vardimid(parent_group_id, variable_id, dim_ids.data()));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to read variable dimension IDs for variable {}.\n", get_variable_path(parent_group_id, variable_id));
    }

    return dim_ids;
}

std::vector<size_t> NetCDFReader::get_variable_dimension_lengths(const int32_t parent_group_id, const int32_t variable_id) const
{
    std::vector<int32_t> dim_ids = get_variable_dimension_ids(parent_group_id, variable_id);
    std::vector<size_t> dim_lengths;
    for (const auto& dim_id : dim_ids)
    {
        dim_lengths.push_back(get_dimension_length(parent_group_id, dim_id));
    }

    return dim_lengths;
}

std::vector<std::string> NetCDFReader::get_variable_dimension_names(const int32_t parent_group_id, const int32_t variable_id) const
{
    std::vector<std::string> dim_names;

    for (const auto& dim_id : get_variable_dimension_ids(parent_group_id, variable_id)) 
    {
        dim_names.push_back(get_dimension_name(parent_group_id, dim_id));
    }

    return dim_names;
}

size_t NetCDFReader::get_variable_number_of_dimensions(const int32_t parent_group_id, const int32_t variable_id) const
{
    int32_t n_dims;
    int32_t status = static_cast<int32_t>(nc_inq_varndims(parent_group_id, variable_id, &n_dims));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to read variable number of dimensions for variable {}.\n", get_variable_path(parent_group_id, variable_id));
    }

    return n_dims;
}

size_t NetCDFReader::get_dimension_length(const int32_t parent_group_id, const int32_t dimension_id) const
{
    size_t length;

    int32_t status = static_cast<int32_t>(nc_inq_dimlen(parent_group_id, dimension_id, &length));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to read dimension length for dimension ID {}.\n", dimension_id);
    }

    return length;
}

std::string NetCDFReader::get_dimension_name(const int32_t parent_group_id, const int32_t dimension_id) const
{
    std::array<char, NC_MAX_NAME + 1> c_str;

    int32_t status = static_cast<int32_t>(nc_inq_dimname(parent_group_id, dimension_id, c_str.data()));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to find name for dimension id {}.\n", dimension_id);
        return "NO_NAME";
    }

    return c_str.data();
}

std::string NetCDFReader::get_type(const int32_t parent_group_id, const int32_t variable_id) const
{
    nc_type var_type;
    int32_t status = static_cast<int32_t>(nc_inq_vartype(parent_group_id, variable_id, &var_type));

    if (status != NC_NOERR)
    {
        fmt::print("Unable to find NC type for variable {}.\n", get_variable_path(parent_group_id, variable_id));
        return "NO_NAME";
    }

    std::array<char, NC_MAX_NAME + 1> c_str;
    status = nc_inq_type(variable_id, var_type, c_str.data(), nullptr);

    if (status != NC_NOERR)
    {
        fmt::print("Unable to find type name for variable {}.\n", get_variable_path(parent_group_id, variable_id));
        return "NO_NAME";
    }

    return c_str.data();
}

double NetCDFReader::get_fill_value(const int32_t parent_group_id, const int32_t variable_id) const
{
    double fill_value;

    int32_t status = nc_get_att_double(parent_group_id, variable_id, "_FillValue", &fill_value);

    if (status != NC_NOERR)
    {
        fill_value = NC_FILL_DOUBLE;
    }

    return fill_value;
}

void NetCDFReader::read_into_group_nodes(GroupNode& group)
{
    read_into_group_nodes(group, this->root_grp_id);
}

void NetCDFReader::read_into_group_nodes(GroupNode& group, int32_t group_id)
{
    group.id = group_id;

    if (group.is_root)
    {
        group.name = path.filename().string();
        group.root_id = group.id;
    }
    else
    {
        group.name = get_group_name(group_id);
    }

    group.child_variable_ids = get_variable_ids(group_id);
    group.child_variable_names = get_variable_names(group_id);
    for (size_t i = 0; i < group.child_variable_ids.size(); i++)
    {
        group.variable_children.push_back(std::make_unique<VariableNode>());
        auto& variable_node = group.variable_children.back();
        variable_node->id = group.child_variable_ids[i];
        variable_node->parent_group_id = group.id;
        variable_node->root_id = group.root_id;
        variable_node->name = group.child_variable_names[i];
    }

    std::vector<int32_t> child_group_ids = get_group_ids(group_id);
    group.group_children.clear();
    for (const auto &child_group_id : child_group_ids)
    {
        group.group_children.push_back(std::make_unique<GroupNode>());
        auto &child_group = *group.group_children.back();
        child_group.root_id = group.root_id;
        read_into_group_nodes(child_group, child_group_id);
    }
}