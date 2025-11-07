/*
Mesh.h
James Hocking, 2025
*/

#pragma once

#include "Helpers.h"
#include "Light.h"
#include "Image.h"
#include <Eigen/Dense>
#include <array>
#include <iostream>
#include <memory>
#include <fstream>
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
    virtual Eigen::Vector3f get_centroid() = 0;
    virtual Eigen::Vector3f get_min_bound() = 0;
    virtual Eigen::Vector3f get_max_bound() = 0;
    virtual std::unique_ptr<Mesh> clone() const = 0;
    std::string get_name() { return _name; };
    virtual ~Mesh() = default;

  protected:
    std::string _name;
    enum MeshType _type;
};

class Cube : public Mesh {
  public:
    Cube(Eigen::Vector3f translation, Eigen::Vector3f rotation, Eigen::Vector3f scale, std::string name, enum MeshType type) : Mesh(std::move(name), type), _translation(translation), _rotation(rotation), _scale(scale) {};
    
    // print out the properties of the cube to std output 
    void show_properties() override;

    /*
    Function to find the intersection between a ray and a cube. This is 
    done by transforming the ray into the basis of the cube then running 
    the 'slab test'.
    */
    bool check_intersect(Ray &r, Hit *hit) override;

    // getters/setters
    enum MeshType get_meshtype() { return MeshType::CUBE; };

    Eigen::Vector3f get_centroid() {return _translation;};

    Eigen::Vector3f get_min_bound() {return _translation - Eigen::Vector3f(_scale[0], _scale[1], _scale[2]);};

    Eigen::Vector3f get_max_bound() {return _translation + Eigen::Vector3f(_scale[0], _scale[1], _scale[2]);};

    std::unique_ptr<Mesh> clone() const override {return std::make_unique<Cube>(*this);}

  private:
    Eigen::Vector3f _translation;
    Eigen::Vector3f _rotation;
    Eigen::Vector3f _scale;
};

class Sphere : public Mesh {
  public:
    Sphere(Eigen::Vector3f location, Eigen::Vector3f rotation, Eigen::Vector3f scale, std::string name, enum MeshType type) : Mesh(std::move(name), type), _location(location), _rotation(rotation), _scale(scale) {};
    
    // print out the properties of the sphere to the std output 
    void show_properties() override;

    /*
    Function in order to find the intersection between a ray and a sphere. This 
    function was copied from:
    https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
    which describes the geometric reasoning used to solve for the intersection point of the two shapes.
    */
    bool check_intersect(Ray &r, Hit *hit) override;

    // getters/setters
    enum MeshType get_meshtype() override { return MeshType::SPHERE;};

    Eigen::Vector3f get_centroid() {return _location;};
    
    Eigen::Vector3f get_min_bound() {return _location - Eigen::Vector3f(_scale[0], _scale[1], _scale[2]);};  
    
    Eigen::Vector3f get_max_bound() {return _location + Eigen::Vector3f(_scale[0], _scale[1], _scale[2]);};

    std::unique_ptr<Mesh> clone() const override {return std::make_unique<Sphere>(*this);}

  private:
    Eigen::Vector3f _location;
    Eigen::Vector3f _rotation;
    Eigen::Vector3f _scale;
};

class Plane : public Mesh {
  public:
    Plane(const std::array<Eigen::Vector3f, NUMBER_OF_PLANE_CORNERS> &corners, std::string name, MeshType type): Mesh(std::move(name), type), _corners(corners) {
      // find a point on the plane and a normal vector
      _point = _corners[PlaneCorners::BOTTOM_LEFT];
      _normal = (corners[PlaneCorners::BOTTOM_LEFT] - corners[PlaneCorners::BOTTOM_RIGHT]).cross((corners[PlaneCorners::BOTTOM_LEFT] - corners[PlaneCorners::TOP_LEFT])).normalized();
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

    Eigen::Vector3f get_centroid() {
      return _corners[PlaneCorners::BOTTOM_LEFT] + (_corners[PlaneCorners::TOP_RIGHT] - _corners[PlaneCorners::BOTTOM_LEFT]);
    };

      Eigen::Vector3f get_min_bound() {
        Eigen::Vector3f min_bound = _corners[0];
        for (int i = 1; i < NUMBER_OF_PLANE_CORNERS; i++) {
            min_bound = min_bound.cwiseMin(_corners[i]);
        }
        return min_bound;
    };
    
    Eigen::Vector3f get_max_bound() {
        Eigen::Vector3f max_bound = _corners[0];
        for (int i = 1; i < NUMBER_OF_PLANE_CORNERS; i++) {
            max_bound = max_bound.cwiseMax(_corners[i]);
        }
        return max_bound;
    };

    std::unique_ptr<Mesh> clone() const override {return std::make_unique<Plane>(*this);}

  private:
    std::array<Eigen::Vector3f, NUMBER_OF_PLANE_CORNERS> _corners;
    Eigen::Vector3f _point;
    Eigen::Vector3f _normal;
};