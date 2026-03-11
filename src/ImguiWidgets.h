#ifndef IMGUIWIDGETS_H
#define IMGUIWIDGETS_H

#include <memory>

#include "imgui.h"
#include "Node.h"
#include "PlotData.h"
#include "VariableData.h"
#include "NetCDFReader.h"

class ImguiWidgets
{
public:
    static void traverse_groups_for_ui(GroupNode& group, VariableNode &selected_variable, bool &new_selection);
    static bool file_hierarchy(std::vector<std::unique_ptr<GroupNode>>& root_groups, VariableNode &selected_variable);
    static bool variable_window(VariableData& var_data, PlotData& plot_data, NetCDFReader& nc_reader, VariableNode& selected_variable, const bool new_selection = false);
    static void plot_window(PlotData& plot_data, const bool plot_settings_update = false);
    static bool plot_settings(PlotData& plot_data, VariableData& var_data, NetCDFReader& nc_reader, VariableNode& selected_variable, const bool new_selection = false);
    static void variable_info(VariableData& var_data);
};

#endif // IMGUIWIDGETS_H