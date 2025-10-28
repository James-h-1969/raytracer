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

bool Cube::check_intersect(Ray& ray, Hit* hit) {
    // Transform ray into local space
    Ray local_ray;
    Eigen::Matrix3f R_inv = euler_to_matrix(_rotation).transpose();
    local_ray.origin = R_inv * (ray.origin - _translation);
    local_ray.direction = R_inv * ray.direction;

    // Slab test in local space 
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax =  std::numeric_limits<float>::infinity();

    Eigen::Vector3f half_size(_scale, _scale, _scale);
    Eigen::Vector3f box_min = -half_size;
    Eigen::Vector3f box_max =  half_size;

    for (int i = 0; i < NUMBER_OF_AXIS; ++i) {
        if (fabs(local_ray.direction[i]) < 1e-6f) {
            if (local_ray.origin[i] < box_min[i] || local_ray.origin[i] > box_max[i])
                return false;
        } else {
            float t1 = (box_min[i] - local_ray.origin[i]) / local_ray.direction[i];
            float t2 = (box_max[i] - local_ray.origin[i]) / local_ray.direction[i];
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }

    if (tmax < 0) return false;  // cube is behind ray

    // Compute intersection point
    float t_hit = tmin >= 0 ? tmin : tmax; // take the first valid intersection
    Eigen::Vector3f local_hit = local_ray.origin + t_hit * local_ray.direction;
    hit->intersection_point = euler_to_matrix(_rotation) * local_hit + _translation; // back to world space
    hit->distance_along_ray = t_hit;
    hit->is_hit = true;

    return true;
}

bool Sphere::check_intersect(Ray& ray, Hit* hit) {   
    // find the two intersection points of the ray and the sphere 
    float t0, t1; 
    Eigen::Vector3f L = _location - ray.origin;
    float tca = L.dot(ray.direction);
    if (tca < 0) return false;
    float d2 = L.dot(L) - tca * tca;
    if (d2 > _radius * _radius) return false;
    float thc = std::sqrt(_radius * _radius - d2);
    t0 = tca - thc;
    t1 = tca + thc;
    
    // find which intersection is closer 
    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1;
        if (t0 < 0) return false;
    }
    
    hit->intersection_point = ray.origin + t0 * ray.direction;
    hit->distance_along_ray = t0;
    hit->is_hit = true;

    return true;
}

bool Plane::check_intersect(Ray& ray, Hit* hit) {
    float denom = ray.direction.dot(_normal);
    if (std::abs(denom) < 1e-6f) return false; // parallel

    Eigen::Vector3f p0l0 = _point - ray.origin;
    float t = p0l0.dot(_normal) / denom;
    if (t < 0) return false; // behind ray

    Eigen::Vector3f ip = ray.origin + t * ray.direction;

    // check that the intersection point is within the bounds of the plane
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

    if (x_check && y_check && z_check) {
        hit->intersection_point = ip;
        hit->distance_along_ray = t;
        hit->is_hit = true;
        return true;
    }

    return false;
}
