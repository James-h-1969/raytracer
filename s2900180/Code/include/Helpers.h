#pragma once

#include <Eigen/Dense>
#include <nlohmann/json.hpp>

inline Eigen::Matrix3f euler_to_matrix(Eigen::Vector3f euler) {
  /*
  Function that turns euler angle rotation into a 3x3 rotation matrix.
  */
  using Eigen::AngleAxisf;
  using Eigen::Vector3f;
  using Eigen::Matrix3f;

  // Create angle-axis rotations for each local axis rotation
  AngleAxisf rotX(euler.x(), Vector3f::UnitX());
  AngleAxisf rotY(euler.y(), Vector3f::UnitY());
  AngleAxisf rotZ(euler.z(), Vector3f::UnitZ());


  Matrix3f R = (rotZ * rotY * rotX).toRotationMatrix();
  return R;
}

inline Eigen::Vector3f vec3_from_json(const nlohmann::json &j) {
  /*
  Function that turns a json object that is assumed to come in as a list with three parts 
  and turns it into a Eigen::Vector3f 
  */
  return Eigen::Vector3f(j[0], j[1], j[2]);
}
