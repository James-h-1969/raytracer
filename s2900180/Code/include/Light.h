/*
Light.h 
James Hocking, 2025
*/

#pragma once 

#include <Eigen/Dense>
#include "Camera.h"
#include <iostream>

struct Ray {
    Eigen::Vector3f origin;
    Eigen::Vector3f direction;

    void print() {
        std::cout << "Origin:\n" << origin << std::endl;
        std::cout << "Direction:\n" << direction << std::endl;
    };
};

struct Hit {
    Eigen::Vector3f intersection_point;
    Eigen::Vector3f normal;
    float distance_along_ray;
    bool is_hit = false;

    // maybe in the future add texture properties here
};

class Light {
    public:
        Light(Eigen::Vector3f location, float radiant_intensity) : 
            _location(location), _radiant_intensity(radiant_intensity) {;};

        // getters/setters
        Eigen::Vector3f get_location() {return _location;};
        float get_radiant_intensity() {return _radiant_intensity;};

    private:
        Eigen::Vector3f _location; 
        float _radiant_intensity;
};

void update_hit_from_intersection(Hit* h, Eigen::Vector3f intersection_point, Eigen::Vector3f normal, float distance_along_ray);
Eigen::Vector3f shade(Hit* hit, Light* light, CameraProperties* props, float kd, float ks, float shininess, Eigen::Vector3f base_colour);