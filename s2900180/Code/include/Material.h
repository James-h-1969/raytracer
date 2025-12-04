#pragma once 

#include <Eigen/Dense>
#include "Types.h"

class PPMImageFIle;

struct Material { 
  float ka; // constant for ambiance
  float kd; // constant for diffuse
  float ks; // constant for specular
  float shininess;
  float reflectivity;
  float transparancy = 0.0;
  float ior = 1.0; // index of refraction
  Colour base_colour;
  PPMImageFile* texture = nullptr;
};