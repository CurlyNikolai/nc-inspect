#include <fmt/core.h>
#include <fmt/ranges.h>
#include <filesystem>
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "Screen.h"
#include "imgui.h"
#include "implot.h"
#include "NetCDFReader.h"
#include "Node.h"
#include "ImguiWidgets.h"
#include "VariableData.h"
#include "PlotData.h"

int main(int argc, char **argv)
{
    size_t arg_count{static_cast<size_t>(argc)};
    
    std::vector<std::filesystem::path> files;
    for (size_t i = 0; i < arg_count; i++)
    {
        if (i > 0)
        {
            std::filesystem::path file_path(argv[i]);
            
            if (!std::filesystem::exists(file_path))
            {
                fmt::print("File {} does not exist.\n", file_path.c_str());
                continue;
            }
            
            if (!std::filesystem::is_regular_file(file_path))
            {
                fmt::print("Provided filename {} is not a regular file.\n", file_path.c_str());
                continue;
            }
            
            if (file_path.extension() != ".nc")
            {
                fmt::print("Provided file {} is not a NetCDF file.\n", file_path.c_str());
                continue;
            }

            files.push_back(std::filesystem::absolute(file_path));
        }
    }
    
    if (files.size() == 0)
    {
        fmt::print("No files provided for inspection, exiting.\n");
        return 0;
    }

    Screen::init(1080, 720);
    
    std::vector<std::unique_ptr<NetCDFReader>> nc_readers;
    std::vector<std::unique_ptr<GroupNode>> root_groups;

    for (const auto &file : files)
    {
        nc_readers.push_back(std::make_unique<NetCDFReader>(file));
        auto& reader = nc_readers.back();

        reader->open();
        root_groups.push_back(std::make_unique<GroupNode>());
        auto &root_group = *root_groups.back();
        root_group.is_root = true;
        reader->read_into_group_nodes(root_group);
    }

    std::unordered_map<std::string, std::vector<std::string>> groups;
    std::unordered_map<std::string, std::vector<std::string>> variables;

    VariableNode selected_variable;
    size_t current_reader_idx = 0;
    VariableData var_data;
    PlotData plot_data;
    double last_update_time{0.0};

    ImPlot::GetStyle().Colormap = ImPlotColormap_Spectral;

    while (!Screen::should_close())
    {
        // Restrict frame update to 144 fps
        double time = glfwGetTime();
        double time_since_last_update = time - last_update_time;
        if (time_since_last_update >= 1.0 / 144.0)
        {
            last_update_time = time;
        }
        else
        {
            continue;
        }

        Screen::frame_start();

        
        // File, group and variable selector
        bool new_selection = ImguiWidgets::file_hierarchy(root_groups, selected_variable);
        if (new_selection)
        {
            const auto &it = std::find_if(nc_readers.begin(), nc_readers.end(), [selected_variable](const auto &reader)
                                          { return reader->root_grp_id == selected_variable.root_id; });
            current_reader_idx = std::distance(nc_readers.begin(), it);

            auto &current_reader = *nc_readers.at(current_reader_idx);
            var_data.name = selected_variable.name;
            var_data.path = current_reader.get_variable_path(selected_variable.parent_group_id, selected_variable.id);
            var_data.type = current_reader.get_type(selected_variable.parent_group_id, selected_variable.id);
            var_data.dimension_names = current_reader.get_variable_dimension_names(selected_variable.parent_group_id, selected_variable.id);
            var_data.dimenson_lengths = current_reader.get_variable_dimension_lengths(selected_variable.parent_group_id, selected_variable.id);
            var_data.update_dimension_type();

            plot_data.x_data.clear();
            plot_data.y_data.clear();
            plot_data.z_data.clear();

            plot_data.start_indices.resize(var_data.dimension_names.size(), 0);
        }

        bool variable_settings_update = ImguiWidgets::variable_window(var_data, plot_data, *nc_readers[current_reader_idx], selected_variable, new_selection);

        // Variable plot window
        ImguiWidgets::plot_window(plot_data, new_selection || variable_settings_update);

        Screen::frame_end();
    }

    for (auto &reader : nc_readers)
    {
        reader->close();
    }

    Screen::clean_up();

    return 0;
}