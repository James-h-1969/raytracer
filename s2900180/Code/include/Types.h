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

struct Colour {
    int r = 0;
    int g = 0;
    int b = 0;
};