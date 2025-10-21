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
    for (int i = 0; i < NUMBER_OF_PLANE_CORNERS; ++i) {
        std::cout << " Corner " << i + 1 << ": "
                  << _corners[i].transpose() << "\n";
    }
}

bool Cube::check_intersect(Ray& ray, Eigen::Vector3f* intersection_point) {
    // TODO. implement this 
    return false;
}

bool Sphere::check_intersect(Ray& ray, Eigen::Vector3f* intersection_point) {
    // TODO. implement this  
    std::cout << "TESTING2" << std::endl;  
    return false;
}

bool Plane::check_intersect(Ray& ray, Eigen::Vector3f* intersection_point) {
    float denom = ray.direction.dot(_normal);
    if (std::abs(denom) < 1e-6f) return false; // parallel

    Eigen::Vector3f p0l0 = _point - ray.origin;
    float t = p0l0.dot(_normal) / denom;
    if (t < 0) return false; // behind ray

    Eigen::Vector3f ip = ray.origin + t * ray.direction;

    auto same_sign = [](float a, float b, float tol = 1e-5f) {
        if (std::abs(a) < tol) a = 0.0f;
        if (std::abs(b) < tol) b = 0.0f;

        return (a >= 0 && b >= 0) || (a <= 0 && b <= 0);
    };

    Eigen::Vector3f v1 = ip - _corners[0];
    Eigen::Vector3f v2 = _corners[3] - ip;

    bool x_check = same_sign(v1[0], v2[0]);
    bool y_check = same_sign(v1[1], v2[1]);
    bool z_check = same_sign(v1[2], v2[2]);

    *intersection_point = ip;

    return (x_check && y_check && z_check);
}