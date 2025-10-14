/*
Mesh.h 
James Hocking, 2025
*/

#pragma once 

#include <Eigen/Dense>
#include <string>
#include <array>
#include <iostream>

enum class MeshType {
    CUBE,
    SPHERE,
    PLANE,
};

class Mesh {
    public:
        Mesh(std::string name, MeshType type)
        : _name(std::move(name)), _type(type) {}
        virtual void show_properties() = 0;
        virtual ~Mesh() = default;
    protected:
        std::string _name;
        enum MeshType _type;
};

class Cube: public Mesh {
    public:
        Cube(Eigen::Vector3f translation, Eigen::Vector3f rotation, float scale, std::string name, enum MeshType type) : 
            Mesh(std::move(name), type), _translation(translation), _rotation(rotation), _scale(scale) {};
        void show_properties() override;
    private:
        Eigen::Vector3f _translation;
        Eigen::Vector3f _rotation;
        float _scale;
};

class Sphere: public Mesh {
    public:
        Sphere(Eigen::Vector3f location, float radius, std::string name, enum MeshType type) : Mesh(std::move(name), type),_location(location), _radius(radius) {};
        void show_properties() override;
    private:
        Eigen::Vector3f _location;
        float _radius;
};

class Plane : public Mesh {
    public:
        Plane(const std::array<Eigen::Vector3f, 4>& corners,
            std::string name,
            MeshType type)
            : Mesh(std::move(name), type), _corners(corners) {}

        void show_properties() override;

    private:
        std::array<Eigen::Vector3f, 4> _corners;
};