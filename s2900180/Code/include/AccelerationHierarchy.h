/*
AccelerationHierarchy.h 
James Hocking, 2025
*/

#pragma once 

#include <Eigen/Dense>

class BoundingBoxNode {
    public:
        bool check_intersect(Ray ray);
    private:
        Eigen::Vector3f _min; 
        Eigen::Vector3f _max;
};

class BoundingBoxHierarchyTree {
    public:
        BoundingBoxHierarchyTree();
        std::unique_ptr<BoundingBoxNode>& get_root() {return &_root;};
    private:
        std::unique_ptr<BoundingBoxNode> _root;
};