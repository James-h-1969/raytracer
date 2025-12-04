#include "AccelerationHierarchy.h"

void find_max_and_min(Eigen::Vector3f& max, Eigen::Vector3f& min, const std::vector<std::unique_ptr<Mesh>>& meshes) {
    // function that finds the global max and min based on the meshes provided
    
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

    max = global_max;
    min = global_min;
}


BoundingBoxHierarchyTree::BoundingBoxHierarchyTree(
    std::vector<std::unique_ptr<Mesh>> meshes
) {
    _root = std::make_unique<BoundingBoxNode>();
    
    find_max_and_min(_root->max, _root->min, meshes);   
    split_bounding_box(_root, meshes, 0);
}

void BoundingBoxHierarchyTree::split_bounding_box(
    std::unique_ptr<BoundingBoxNode>& node, 
    std::vector<std::unique_ptr<Mesh>>& meshes, 
    int depth
) {
    if (meshes.empty()) return;
    
    // if they reach leaf or max depth
    if (meshes.size() <= 2 || depth > MAX_DEPTH) {
        node->meshes = std::move(meshes);
        return;
    }
    
    // find what axis to split on (find the longest)
    Eigen::Vector3f size = node->max - node->min;
    int axis;
    if (size.x() >= size.y() && size.x() >= size.z()) {
        axis = 0;
    } else if (size.y() >= size.z()) {
        axis = 1;
    } else {
        axis = 2;
    }
    float split_pos = node->min[axis] + size[axis] / 2.0f;
    
    // based on the axis, find which should be left and right
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

    // recurse into it
    find_max_and_min(node->left->max, node->left->min, left_meshes);
    find_max_and_min(node->right->max, node->right->min, right_meshes);   
    split_bounding_box(node->left, left_meshes, depth + 1);
    split_bounding_box(node->right, right_meshes, depth + 1);
}

bool BoundingBoxNode::check_intersect(Ray ray, Hit* hit, int* counter) {
    (*counter)++; // purely for testing

    // do slab test
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax =  std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; ++i) {
        if (fabs(ray.direction[i]) < 1e-6f) {
            if (ray.origin[i] < min[i] || ray.origin[i] > max[i])
                return false;
        } else {
            float t1 = (min[i] - ray.origin[i]) / ray.direction[i];
            float t2 = (max[i] - ray.origin[i]) / ray.direction[i];
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }

    if (tmax < 0) return false; 

    // left node, check intersect with actual meshes
    if (!left && !right) {
        bool hit_any = false;
        for (auto& mesh_ptr : meshes) {
            (*counter)++; 
            Mesh* mesh = mesh_ptr.get();   // get raw pointer
            if (mesh->check_intersect(ray, hit)) {
                hit_any = true;
            }
        }
        return hit_any;
    }

    // interanl node, recurse
    bool hit_left = left && left->check_intersect(ray, hit, counter);
    bool hit_right = right && right->check_intersect(ray, hit, counter);

    return hit_left || hit_right;
}

bool BoundingBoxHierarchyTree::check_intersect(Ray ray, Hit* hit, int* counter){
    return (_root->check_intersect(ray, hit, counter));
}

void print_subtree(const std::unique_ptr<BoundingBoxNode>& node, int depth) {
    if (!node->meshes.empty()) {
        std::cout << "Current Leaf at depth " << depth << " has " 
              << node->meshes.size() << " meshes" << std::endl;
    } else {
        std::cout << "Current Branch at depth " << depth << std::endl;
    }
    
    std::cout << "Min val:\n" << node->min << std::endl;
    std::cout << "Max val:\n" << node->max << "\n\n";
    
    if (node->left) {
        print_subtree(node->left, depth + 1);
    }
    if (node->right) {
        print_subtree(node->right, depth + 1);
    }
}

void BoundingBoxHierarchyTree::print() {
    print_subtree(_root, 0);
}