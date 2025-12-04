#pragma once 

#include "Light.h"
#include <Eigen/Dense>
#include <memory>
#include <vector>
#include "Mesh.h"      
#include <limits> 

// forward declaration
class Mesh;
struct Hit;
struct Ray;

// constants
constexpr int MAX_DEPTH = 10; // representing the depth of the bounding box hierarchy tree

// classes
class BoundingBoxNode {
    /*
        Node with the bounding box tree that contains the meshes within its bounds.

        Common flow is to check intersection, and if true then continue down into tree.
        If not can skip the branches below it completely.
    */
    public:
        // function to check whether a ray is intersecting with the box
        bool check_intersect(Ray ray, Hit* hit, int* counter);
        
        // min and max values of the box
        Eigen::Vector3f min; 
        Eigen::Vector3f max;

        // all the meshes contained within the box
        std::vector<std::unique_ptr<Mesh>> meshes;

        // inner left and inner right nodes
        std::unique_ptr<BoundingBoxNode> left;
        std::unique_ptr<BoundingBoxNode> right;
};

class BoundingBoxHierarchyTree {
    /*
        Tree container class that holds the root of the tree and constructing the 
        tree. 
    */
    public:
        // constructor that holds the meshes
        BoundingBoxHierarchyTree(std::vector<std::unique_ptr<Mesh>> meshes);

        // print out the whole tree
        void print();

        // check the intersection of a ray and the root of the tree
        bool check_intersect(Ray ray, Hit* hit, int* counter);
        
    private:
        // root of the tree
        std::unique_ptr<BoundingBoxNode> _root;
        
        // helper function for the constructing of the tree. Given a box, creates its left and right children.
        void split_bounding_box(std::unique_ptr<BoundingBoxNode>& node, 
                            std::vector<std::unique_ptr<Mesh>>& meshes, 
                            int depth);

};