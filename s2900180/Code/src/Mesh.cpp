#include "Mesh.h"

void Cube::show_properties() {
    std::cout << "\nMesh: " << _name << "\n";
    std::cout << "Type: Cube\n";
    std::cout << "Translation: " << _translation.transpose() << "\n";
    std::cout << "Rotation: " << _rotation.transpose() << "\n";
    std::cout << "Scale: " << _scale.transpose() << "\n";
}

void Sphere::show_properties() {
    std::cout << "\nMesh: " << _name << "\n";
    std::cout << "Type: Sphere\n";
    std::cout << "Location: " << _location.transpose() << "\n";
    std::cout << "Rotation: " << _rotation.transpose() << "\n";
    std::cout << "Scale: " << _scale.transpose() << "\n";
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
    // Transform ray into local object space
    Eigen::Matrix3f R_inv = euler_to_matrix(_rotation).transpose();
    Eigen::Vector3f local_origin = R_inv * (ray.origin - _translation);
    Eigen::Vector3f local_dir = R_inv * ray.direction;

    // Half extents per axis (non-uniform scale)
    Eigen::Vector3f half_size = _scale;

    Eigen::Vector3f box_min = -half_size;
    Eigen::Vector3f box_max =  half_size;

    // Slab test
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax =  std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; ++i) {
        if (fabs(local_dir[i]) < 1e-6f) {
            // Ray is parallel to slab
            if (local_origin[i] < box_min[i] || local_origin[i] > box_max[i])
                return false;
        } else {
            float t1 = (box_min[i] - local_origin[i]) / local_dir[i];
            float t2 = (box_max[i] - local_origin[i]) / local_dir[i];
            if (t1 > t2) std::swap(t1, t2);

            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }

    if (tmax < 0) return false; // Box is behind the ray

    // Compute intersection point in local space
    float t_hit = tmin >= 0 ? tmin : tmax;
    Eigen::Vector3f local_hit = local_origin + t_hit * local_dir;

    // Transform intersection back to world space
    hit->intersection_point = euler_to_matrix(_rotation) * local_hit + _translation;
    hit->distance_along_ray = t_hit;
    hit->is_hit = true;

    return true;
}


bool Sphere::check_intersect(Ray& ray, Hit* hit) {
    // Step 1: Transform ray into local object space
    Eigen::Matrix3f R_inv = euler_to_matrix(_rotation).transpose();
    Eigen::Vector3f local_origin = R_inv * (ray.origin - _location);
    Eigen::Vector3f local_dir = R_inv * ray.direction;

    // Step 2: Apply inverse scale (turn ellipsoid into a unit sphere)
    Eigen::Vector3f inv_scale = _scale.cwiseInverse();
    local_origin = local_origin.cwiseProduct(inv_scale);
    local_dir = local_dir.cwiseProduct(inv_scale);

    // Step 3: Ray-sphere intersection in unit sphere space
    float a = local_dir.dot(local_dir);
    float b = 2.0f * local_origin.dot(local_dir);
    float c = local_origin.dot(local_origin) - 1.0f; // radius = 1 after scaling

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) return false;

    float sqrt_disc = std::sqrt(discriminant);
    float t0 = (-b - sqrt_disc) / (2.0f * a);
    float t1 = (-b + sqrt_disc) / (2.0f * a);
    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1;
        if (t0 < 0) return false;
    }

    // Step 4: Compute intersection point in local space
    Eigen::Vector3f local_hit = local_origin + t0 * local_dir;

    // Step 5: Transform back to world space
    Eigen::Vector3f hit_world = (euler_to_matrix(_rotation) * 
                                (local_hit.cwiseProduct(_scale))) + _location;

    hit->intersection_point = hit_world;
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
