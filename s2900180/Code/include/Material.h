#pragma once 

#include <Eigen/Dense>
#include "Types.h"

class PPMImageFIle;

struct Material { 
  float ka; // constant for ambiance
  Eigen::Vector3f kd; // constant for diffuse
  Eigen::Vector3f ks; // constant for specular
  float shininess;
  float reflectivity;
  Colour base_colour;
  PPMImageFile* texture = nullptr;
};