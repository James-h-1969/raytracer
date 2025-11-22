#pragma once 

#include <Eigen/Dense>
#include <nlohmann/json.hpp>

inline Eigen::Matrix3f euler_to_matrix(Eigen::Vector3f euler) {
    /*
    Function that turns Euler angles into a rotation matrix
    */
    
    float cx = cos(euler.x());
    float sx = sin(euler.x());
    float cy = cos(euler.y());
    float sy = sin(euler.y());
    float cz = cos(euler.z());
    float sz = sin(euler.z());

    Eigen::Matrix3f Rx;
    Rx << 1, 0, 0,
          0, cx, -sx,
          0, sx, cx;

    Eigen::Matrix3f Ry;
    Ry << cy, 0, sy,
          0, 1, 0,
          -sy, 0, cy;

    Eigen::Matrix3f Rz;
    Rz << cz, -sz, 0,
          sz, cz, 0,
          0, 0, 1;

    return Rx * Ry * Rz;
}

inline Eigen::Vector3f vec3_from_json(const nlohmann::json& j) {
      return Eigen::Vector3f(j[0], j[1], j[2]);
}
