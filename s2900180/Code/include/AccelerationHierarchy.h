#pragma once 


#include "Light.h"
#include <Eigen/Dense>
#include <memory>
#include <vector>
#include "Mesh.h"      
#include <limits> 

class Mesh; // Forward declaration

constexpr int MAX_DEPTH = 10;

class BoundingBoxNode {
public:
    bool check_intersect(Ray ray, Hit* hit, int* counter);
    
    Eigen::Vector3f min; 
    Eigen::Vector3f max;
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::unique_ptr<BoundingBoxNode> left;
    std::unique_ptr<BoundingBoxNode> right;
};

class BoundingBoxHierarchyTree {
public:
    BoundingBoxHierarchyTree(std::vector<std::unique_ptr<Mesh>> meshes);

    void print();

    bool check_intersect(Ray ray, Hit* hit, int* counter);
    
private:
    std::unique_ptr<BoundingBoxNode> _root;
    
    void split_bounding_box(std::unique_ptr<BoundingBoxNode>& node, 
                           std::vector<std::unique_ptr<Mesh>>& meshes, 
                           int depth);

};