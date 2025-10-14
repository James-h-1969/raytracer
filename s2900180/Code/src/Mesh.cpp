#include "Mesh.h"

void Cube::show_properties() {
    std::cout << "\nMesh: " << _name << "\n";
    std::cout << "Type: Cube\n";
    std::cout << "Translation: " << _translation.transpose() << "\n";
    std::cout << "Rotation: " << _rotation.transpose() << "\n";
    std::cout << "Scale: " << _scale << "\n";
}

void Sphere::show_properties() {
    std::cout << "\nMesh: " << _name << "\n";
    std::cout << "Type: Sphere\n";
    std::cout << "Location: " << _location.transpose() << "\n";
    std::cout << "Radius: " << _radius << "\n";
}

void Plane::show_properties() {
    std::cout << "\nMesh: " << _name << "\n";
    std::cout << "Type: Plane\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << " Corner " << i + 1 << ": "
                  << _corners[i].transpose() << "\n";
    }
}
