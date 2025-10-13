/*
Light.h 
James Hocking, 2025
*/

#pragma once 

#include <Eigen/Dense>

class Light {
    public:
        Light(Eigen::Vector3f location, float radiant_intensity) : 
            _location(location), _radiant_intensity(radiant_intensity) {}
    private:
        Eigen::Vector3f _location; 
        float _radiant_intensity;
};