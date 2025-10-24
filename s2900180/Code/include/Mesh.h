/*
Mesh.h
James Hocking, 2025
*/

#pragma once

#include "Helpers.h"
#include "Image.h"
#include <Eigen/Dense>
#include <array>
#include <iostream>
#include <string>

constexpr int NUMBER_OF_PLANE_CORNERS = 4;
constexpr int NUMBER_OF_AXIS = 3;

enum PlaneCorners {
  BOTTOM_LEFT,
  BOTTOM_RIGHT,
  TOP_LEFT,
  TOP_RIGHT,
};

enum class MeshType {
  CUBE,
  SPHERE,
  PLANE,
};

class Mesh {
public:
  Mesh(std::string name, MeshType type) : _name(std::move(name)), _type(type) {}
  virtual void show_properties() = 0;
  virtual enum MeshType get_meshtype() = 0;
  virtual bool check_intersect(Ray &r, Hit *hit) = 0;
  std::string get_name() { return _name; };
  virtual ~Mesh() = default;

protected:
  std::string _name;
  enum MeshType _type;
};

class Cube : public Mesh {
public:
  Cube(Eigen::Vector3f translation, Eigen::Vector3f rotation, float scale,
       std::string name, enum MeshType type)
      : Mesh(std::move(name), type), _translation(translation),
        _rotation(rotation), _scale(scale) {};
  void show_properties() override;
  enum MeshType get_meshtype() { return MeshType::CUBE; };
  bool check_intersect(Ray &r, Hit *hit) override;

private:
  Eigen::Vector3f _translation;
  Eigen::Vector3f _rotation;
  float _scale;
};

class Sphere : public Mesh {
public:
  Sphere(Eigen::Vector3f location, float radius, std::string name,
         enum MeshType type)
      : Mesh(std::move(name), type), _location(location), _radius(radius) {};
  void show_properties() override;
  enum MeshType get_meshtype() { return MeshType::SPHERE; };
  bool check_intersect(Ray &r, Hit *hit) override;

private:
  Eigen::Vector3f _location;
  float _radius;
};

class Plane : public Mesh {
public:
  Plane(const std::array<Eigen::Vector3f, NUMBER_OF_PLANE_CORNERS> &corners,
        std::string name, MeshType type)
      : Mesh(std::move(name), type), _corners(corners) {
    // find a point on the plane and a normal vector
    _point = _corners[PlaneCorners::BOTTOM_LEFT];
    _normal = (corners[PlaneCorners::BOTTOM_LEFT] -
               corners[PlaneCorners::BOTTOM_RIGHT])
                  .cross((corners[PlaneCorners::BOTTOM_LEFT] -
                          corners[PlaneCorners::TOP_LEFT]))
                  .normalized();
  }

  // print out the properties of the plane to the std output
  void show_properties() override;

  /*
  Function in order to find the intersection between a ray and a plane. To do
  this, the function first checks the ray is not parallel to the plane. It then
  checks that it is not in the opposite direction (meaning it has to hit on an
  infinite plane based on the normal and point), but then checks that the point
  is within the bounds of the 4 corners defined.
  */
  bool check_intersect(Ray &r, Hit *hit) override;

  // getters/setters
  enum MeshType get_meshtype() { return MeshType::PLANE; };

private:
  std::array<Eigen::Vector3f, NUMBER_OF_PLANE_CORNERS> _corners;
  Eigen::Vector3f _point;
  Eigen::Vector3f _normal;
};
