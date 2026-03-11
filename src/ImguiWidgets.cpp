#include <implot.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <numeric>
#include <algorithm>

#include "ImguiWidgets.h"

void ImguiWidgets::traverse_groups_for_ui(GroupNode& group, VariableNode &selected_variable, bool &new_selection)
{
    if (ImGui::TreeNode(group.name.c_str()))
    {
        for (auto &child_group : group.group_children)
        {
            traverse_groups_for_ui(*child_group, selected_variable, new_selection);
        }

        for (auto &variable : group.variable_children)
        {
            // Select new variable
            if (ImGui::Selectable(variable->name.c_str(), variable->selected))
            {
                selected_variable.parent_group_id = group.id;
                selected_variable.id = variable->id;
                selected_variable.root_id = group.root_id;
                new_selection = true;
                variable->selected = true;
            }
            // Deselect previously selected variable
            if (variable->selected && !(selected_variable.id == variable->id && selected_variable.parent_group_id == group.id && selected_variable.root_id == group.root_id))
            {
                variable->selected = false;
            }
        }

        ImGui::TreePop();
    }
}

bool ImguiWidgets::file_hierarchy(std::vector<std::unique_ptr<GroupNode>>& root_groups, VariableNode &selected_variable)
{
    ImGui::Begin("Files");
    bool new_selection{false};
    for (size_t i = 0; i < root_groups.size(); i++)
    {
        ImguiWidgets::traverse_groups_for_ui(*root_groups.at(i), selected_variable, new_selection);
    }
    ImGui::End();
    return new_selection;
}

bool ImguiWidgets::variable_window(VariableData &var_data, PlotData &plot_data, NetCDFReader &nc_reader, VariableNode &selected_variable, const bool new_selection)
{
    bool update{false};
    ImGui::Begin("Variable");
    if (selected_variable.id != -1)
    {
        // Variable info section
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImguiWidgets::variable_info(var_data);

        // Plot settings section
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        update = ImguiWidgets::plot_settings(plot_data, var_data, nc_reader, selected_variable, new_selection);
    }
    ImGui::End();
    return update;
}

void ImguiWidgets::plot_window(PlotData &plot_data, const bool plot_settings_update)
{
    ImGui::Begin("Variable Plot", nullptr, ImGuiWindowFlags_MenuBar);
    static ImVec4 line_color {0.370f, 0.433f, 0.564f, 1.000f};
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Settings"))
        {
            static int32_t idx{0};
            if (ImGui::BeginMenu("Heatmap"))
            {
                if (ImGui::BeginMenu("Colormap"))
                {
                    if (ImGui::MenuItem("Spectral"))
                    {
                        ImPlot::GetStyle().Colormap = ImPlotColormap_Spectral;
                    }
                    if (ImGui::MenuItem("Viridis"))
                    {
                        ImPlot::GetStyle().Colormap = ImPlotColormap_Viridis;
                    }
                    if (ImGui::MenuItem("BrBG"))
                    {
                        ImPlot::GetStyle().Colormap = ImPlotColormap_BrBG;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Line"))
            {
                ImGui::ColorEdit4("Color", &line_color.x);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (ImPlot::BeginPlot("Plot", ImVec2(-1, -1), ImPlotFlags_NoLegend))
    {
        if (plot_settings_update)
        {
            ImPlot::SetupAxes(plot_data.x_name.c_str(), plot_data.y_name.c_str(), ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        }
        else
        {
            ImPlot::SetupAxes(plot_data.x_name.c_str(), plot_data.y_name.c_str());
        }

        switch (plot_data.type)
        {
        case PlotData::Type::Line:
            ImPlot::SetNextLineStyle(line_color);
            ImPlot::PlotLine("", plot_data.x_data.data(), plot_data.y_data.data(), plot_data.y_data.size());
            break;
            case PlotData::Type::Scatter:
            ImPlot::SetNextMarkerStyle(-1, -1.0f, line_color, -1.0f, line_color);
            ImPlot::PlotScatter("", plot_data.x_data.data(), plot_data.y_data.data(), plot_data.y_data.size());
            break;
        case PlotData::Type::Heatmap:
            ImPlot::PlotHeatmap("", plot_data.z_data.data(), plot_data.x_data.size(), plot_data.y_data.size(), 0, 0, nullptr);
            break;
        default:
            break;
        }
        ImPlot::EndPlot();      
    }

    ImGui::End();
}

bool ImguiWidgets::plot_settings(PlotData &plot_data, VariableData &var_data, NetCDFReader &nc_reader, VariableNode &selected_variable, const bool new_selection)
{
    bool update{false};

    // Variable plotting settings
    ImGui::SeparatorText("Plotting");

    std::vector<std::string> plot_type_names {"None"};
    if (var_data.dimension_type == VariableData::DimensionType::OneDimensional)
    {
        plot_type_names.insert(plot_type_names.end(), {"Line", "Scatter"});
    }
    else if (var_data.dimension_type == VariableData::DimensionType::MultiDimensional)
    {
        plot_type_names.insert(plot_type_names.end(), {"Line", "Scatter", "Heatmap"});
    }

    std::string plot_types = "";
    for (const auto &name : plot_type_names)
    {
        plot_types += name;
        plot_types.push_back('\0');
    }

    if (!plot_types.empty())
    {
        static int32_t plot_type{0};

        if (new_selection)
        {
            plot_type = 0;
            plot_data.set_type(plot_type_names.at(plot_type));
        }

        if (ImGui::Combo("Type", &plot_type, plot_types.c_str()))
        {
            plot_data.set_type(plot_type_names.at(plot_type));
            update = true;
            
            plot_data.x_dim = 0;
            plot_data.y_dim = 1;
  
            if (var_data.dimension_names.size() > 0)
            {
                plot_data.x_name = var_data.dimension_names.at(plot_data.x_dim);
            }
            if (var_data.dimension_names.size() > 1)
            {
                plot_data.y_name = var_data.dimension_names.at(plot_data.y_dim);
            }
        }
    }

    if (plot_data.type == PlotData::Type::Line || plot_data.type == PlotData::Type::Scatter)
    {
        if (new_selection)
        {
            plot_data.x_dim = 0;
        }

        std::string dim_options;
        for (const auto &name : var_data.dimension_names)
        {
            dim_options += name;
            dim_options.push_back('\0');
        }

        if (ImGui::Combo("X-axis", &plot_data.x_dim, dim_options.data()) || update)
        {
            plot_data.y_data.resize(var_data.dimenson_lengths[plot_data.x_dim]);
            plot_data.x_data.resize(var_data.dimenson_lengths[plot_data.x_dim]);
            std::iota(plot_data.x_data.begin(), plot_data.x_data.end(), 0.0);

            std::fill(plot_data.start_indices.begin(), plot_data.start_indices.end(), 0);

            std::vector<size_t> counts(var_data.dimension_names.size(), 1);
            counts[plot_data.x_dim] = var_data.dimenson_lengths.at(plot_data.x_dim);
            nc_reader.read_var_double_data(selected_variable.parent_group_id, selected_variable.id, plot_data.start_indices, counts, plot_data.y_data);

            update = true;

            plot_data.x_name = var_data.dimension_names.at(plot_data.x_dim);
            plot_data.y_name = "";
        }

        for (size_t i = 0; i < var_data.dimension_names.size(); i++)
        {
            const std::string& dim_name = var_data.dimension_names[i];
            if ( !(dim_name == plot_data.x_name) )
            {
                if (ImGui::SliderInt(dim_name.c_str(), &plot_data.start_indices.at(i), 0, var_data.dimenson_lengths[i] - 1))
                {
                    std::vector<size_t> counts(var_data.dimension_names.size(), 1);
                    counts[plot_data.x_dim] = var_data.dimenson_lengths.at(plot_data.x_dim);
                    nc_reader.read_var_double_data(selected_variable.parent_group_id, selected_variable.id, plot_data.start_indices, counts, plot_data.y_data);
                }
            }
        }
    }
    else if (plot_data.type == PlotData::Type::Heatmap)
    {
        if (new_selection)
        {
            plot_data.x_dim = 0;
            plot_data.y_dim = 1;
        }

        std::string dim_options;
        for (const auto &name : var_data.dimension_names)
        {
            dim_options += name;
            dim_options.push_back('\0');
        }
        if (ImGui::Combo("X-axis", &plot_data.x_dim, dim_options.data()))
        {
            if (plot_data.x_dim == plot_data.y_dim)
            {
                plot_data.y_dim = (plot_data.y_dim + 1) % var_data.dimension_names.size();
            }
            plot_data.x_name = var_data.dimension_names.at(plot_data.x_dim);
            plot_data.y_name = var_data.dimension_names.at(plot_data.y_dim);
            std::fill(plot_data.start_indices.begin(), plot_data.start_indices.end(), 0);
            update = true;
        }
        if (ImGui::Combo("Y-axis", &plot_data.y_dim, dim_options.data()))
        {
            if (plot_data.y_dim == plot_data.x_dim)
            {
                plot_data.x_dim = (plot_data.x_dim + 1) % var_data.dimension_names.size();
            }
            plot_data.x_name = var_data.dimension_names.at(plot_data.x_dim);
            plot_data.y_name = var_data.dimension_names.at(plot_data.y_dim);
            std::fill(plot_data.start_indices.begin(), plot_data.start_indices.end(), 0);
            
            update = true;
        }

        for (size_t i = 0; i < var_data.dimension_names.size(); i++)
        {
            const std::string& dim_name = var_data.dimension_names[i];
            if ( dim_name != plot_data.x_name && dim_name != plot_data.y_name )
            {
                if (ImGui::SliderInt(dim_name.c_str(), &plot_data.start_indices.at(i), 0, var_data.dimenson_lengths[i] - 1))
                {
                    update = true;
                }
            }
        }

        if (update)
        {
            plot_data.y_data.resize(var_data.dimenson_lengths[plot_data.y_dim]);
            plot_data.x_data.resize(var_data.dimenson_lengths[plot_data.x_dim]);
            std::iota(plot_data.x_data.begin(), plot_data.x_data.end(), 0.0);
            std::iota(plot_data.y_data.begin(), plot_data.y_data.end(), 0.0);

            std::vector<size_t> counts(var_data.dimension_names.size(), 1);
            counts[plot_data.x_dim] = var_data.dimenson_lengths.at(plot_data.x_dim);
            counts[plot_data.y_dim] = var_data.dimenson_lengths.at(plot_data.y_dim);
            nc_reader.read_var_double_data(selected_variable.parent_group_id, selected_variable.id, plot_data.start_indices, counts, plot_data.z_data, true);
        }
    }

    return update;
}

void ImguiWidgets::variable_info(VariableData &var_data)
{
    ImGui::SeparatorText("Info");

    ImGui::TextUnformatted(fmt::format("Path: {}", var_data.path).c_str());
    ImGui::TextUnformatted(fmt::format("Type: {}", var_data.type).c_str());

    std::string dimension_info{"Dimensions: "};
    for (size_t i = 0; i < var_data.dimension_names.size(); i++)
    {
        dimension_info += fmt::format("{} = {}", var_data.dimension_names[i], var_data.dimenson_lengths[i]);
        if (i < var_data.dimension_names.size() - 1)
        {
            dimension_info += ", ";
        }
    }
    ImGui::TextUnformatted(dimension_info.c_str());
}
