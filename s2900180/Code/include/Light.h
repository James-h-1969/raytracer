/*
Light.h 
James Hocking, 2025
*/

#pragma once 

#include <Eigen/Dense>
#include "Camera.h"
#include "Mesh.h"
#include "AccelerationHierarchy.h"
#include <iostream>
#include <memory>

class Mesh;
class BoundingBoxHierarchyTree;

struct Hit {
    Eigen::Vector3f intersection_point;
    Eigen::Vector3f normal;
    float distance_along_ray;
    bool is_hit = false;

    float u, v; // texture coordinates

    Mesh* mesh;
};

class Light {
    public:
        Light() {;};
        Light(Eigen::Vector3f location, float radiant_intensity) : 
            _location(location), _radiant_intensity(radiant_intensity) {;};

        // getters/setters
        Eigen::Vector3f get_location() {return _location;};
        float get_radiant_intensity() {return _radiant_intensity;};

    private:
        Eigen::Vector3f _location; 
        float _radiant_intensity;
};

void update_hit_from_intersection(Hit* h, Eigen::Vector3f intersection_point, Eigen::Vector3f normal, float distance_along_ray, Mesh* mesh, float u = -1.0f, float v = -1.0f);
Eigen::Vector3f shade(Hit* hit, std::vector<Light> lights, CameraProperties* props, float Ia, std::unique_ptr<BoundingBoxHierarchyTree>& bbht, int depth=0);