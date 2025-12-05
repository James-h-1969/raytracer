/*
Light.h
James Hocking, 2025
*/

#pragma once

#include "AccelerationHierarchy.h"
#include "Camera.h"
#include "Helpers.h"
#include "Mesh.h"
#include <Eigen/Dense>
#include <iostream>
#include <memory>

// forward declaration
class Mesh;
class BoundingBoxHierarchyTree;

// classes
struct Hit {
  /*
    This struct represents the point of intersection between a ray 
    and a mesh. It holds all the important details about this point,
    such as the normal, the mesh (to retrieve the material), the texture 
    coords if applicable
  */
  Eigen::Vector3f intersection_point;
  Eigen::Vector3f normal;
  float distance_along_ray;
  bool is_hit = false;

  float u, v; // texture coordinates

  Mesh *mesh;
};

class Light {
  /*
    This class represents a point light, and holds all the important information 
    for that light.
  */
  public:
    // constructors
    Light() { ; }; 
    Light(Eigen::Vector3f location, float id, float is)
        : _location(location), _id(id), _is(is) {
      ;
    };

    // getters/setters
    Eigen::Vector3f get_location() { return _location; };
    float get_id() { return _id; };
    float get_is() { return _is; };

  private:
    Eigen::Vector3f _location;
    float _id;
    float _is;
};

// helper functions
void update_hit_from_intersection(Hit *h, Eigen::Vector3f intersection_point,
                                  Eigen::Vector3f normal,
                                  float distance_along_ray, Mesh *mesh,
                                  float u = -1.0f, float v = -1.0f);
Eigen::Vector3f shade(Hit *hit, std::vector<Light> lights,
                      CameraProperties *props, float Ia,
                      std::unique_ptr<BoundingBoxHierarchyTree> &bbht,
                      int depth, int max_depth);
