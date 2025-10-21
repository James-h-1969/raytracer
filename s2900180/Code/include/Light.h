/*
Light.h 
James Hocking, 2025
*/

#pragma once 

#include <Eigen/Dense>
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
    float distance_along_ray;
    bool is_hit = false;

    // maybe in the future add texture properties here
};

class Light {
    public:
        Light(Eigen::Vector3f location, float radiant_intensity) : 
            _location(location), _radiant_intensity(radiant_intensity) {}
    private:
        Eigen::Vector3f _location; 
        float _radiant_intensity;
};