#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>
#include <memory>

struct GroupNode
{
    int32_t root_id{-1};
    int32_t id{-1};
    int32_t parent_id{-1};
    std::string name;
    bool is_root{false};
    bool is_open{false};
    std::vector<std::unique_ptr<GroupNode>> group_children;
    std::vector<std::unique_ptr<struct VariableNode>> variable_children;
    std::vector<int32_t> child_variable_ids;
    std::vector<std::string> child_variable_names;
};

struct VariableNode
{
    int32_t root_id{-1};
    int32_t id{-1};
    int32_t parent_group_id{-1};
    std::string name;
    bool selected{false};
};

#endif // NODE_H