#include "AccelerationHierarchy.h"

BoundingBoxHierarchyTree::BoundingBoxHierarchyTree(
    std::vector<std::unique_ptr<Mesh>> meshes
) {
    _root = std::make_unique<BoundingBoxNode>();
    
    Eigen::Vector3f global_min(std::numeric_limits<float>::max(),
                               std::numeric_limits<float>::max(),
                               std::numeric_limits<float>::max());
    Eigen::Vector3f global_max(-std::numeric_limits<float>::max(),
                               -std::numeric_limits<float>::max(),
                               -std::numeric_limits<float>::max());
    
    for (const auto& mesh : meshes) {
        Eigen::Vector3f mesh_min = mesh->get_min_bound();
        Eigen::Vector3f mesh_max = mesh->get_max_bound();
        global_min = global_min.cwiseMin(mesh_min);
        global_max = global_max.cwiseMax(mesh_max);
    }
    
    _root->min = global_min;
    _root->max = global_max;
    
    split_bounding_box(_root, meshes, 0);
}

void BoundingBoxHierarchyTree::split_bounding_box(
    std::unique_ptr<BoundingBoxNode>& node, 
    std::vector<std::unique_ptr<Mesh>>& meshes, 
    int depth
) {
    if (meshes.empty()) return;
    
    if (meshes.size() <= 2 || depth > MAX_DEPTH) {
        node->meshes = std::move(meshes);
        return;
    }
    
    Eigen::Vector3f size = node->max - node->min;
    int axis;
    if (size.x() >= size.y() && size.x() >= size.z()) axis = 0;
    else if (size.y() >= size.z()) axis = 1;
    else axis = 2;
    
    float split_pos = node->min[axis] + size[axis] / 2.0f;
    
    std::vector<std::unique_ptr<Mesh>> left_meshes;
    std::vector<std::unique_ptr<Mesh>> right_meshes;
    
    for (auto& mesh : meshes) {
        float centroid = mesh->get_centroid()[axis];
        if (centroid < split_pos) {
            left_meshes.push_back(std::move(mesh));
        } else {
            right_meshes.push_back(std::move(mesh));
        }
    }
    
    node->left = std::make_unique<BoundingBoxNode>();
    node->right = std::make_unique<BoundingBoxNode>();
    
    node->left->min = node->min;
    node->left->max = node->max;
    node->left->max[axis] = split_pos;
    
    node->right->min = node->min;
    node->right->max = node->max;
    node->right->min[axis] = split_pos;
    
    split_bounding_box(node->left, left_meshes, depth + 1);
    split_bounding_box(node->right, right_meshes, depth + 1);
}