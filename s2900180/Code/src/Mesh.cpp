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
    Eigen::Matrix3f R = euler_to_matrix(_rotation);
    Eigen::Matrix3f R_inv = R.transpose();
    Eigen::Vector3f local_origin = R_inv * (ray.origin - _translation);
    Eigen::Vector3f local_dir = R_inv * ray.direction;

    // Half extents per axis (non-uniform scale / half-size)
    Eigen::Vector3f half_size = _scale; // assume this is half extents
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

    // choose t_hit: if tmin >= 0 -> first hit is at tmin, otherwise ray starts inside so use tmax
    float t_hit_local = (tmin >= 0.0f) ? tmin : tmax;

    // Compute intersection point in local space
    Eigen::Vector3f local_hit = local_origin + t_hit_local * local_dir;

    // Determine which face we hit by checking which coordinate is (within eps) at min/max
    const float EPS = 1e-4f; // tolerance — adjust relative to scale if needed
    Eigen::Vector3f local_normal(0.0f, 0.0f, 0.0f);

    if (std::fabs(local_hit.x() - box_max.x()) < EPS) local_normal = Eigen::Vector3f( 1, 0, 0);
    else if (std::fabs(local_hit.x() - box_min.x()) < EPS) local_normal = Eigen::Vector3f(-1, 0, 0);
    else if (std::fabs(local_hit.y() - box_max.y()) < EPS) local_normal = Eigen::Vector3f(0,  1, 0);
    else if (std::fabs(local_hit.y() - box_min.y()) < EPS) local_normal = Eigen::Vector3f(0, -1, 0);
    else if (std::fabs(local_hit.z() - box_max.z()) < EPS) local_normal = Eigen::Vector3f(0, 0,  1);
    else if (std::fabs(local_hit.z() - box_min.z()) < EPS) local_normal = Eigen::Vector3f(0, 0, -1);
    else {
        // Fallback: find largest component of (local_hit) — avoids degenerate misses
        Eigen::Vector3f abs_hit = local_hit.cwiseAbs();
        if (abs_hit.x() > abs_hit.y() && abs_hit.x() > abs_hit.z())
            local_normal = Eigen::Vector3f((local_hit.x() > 0) ? 1.0f : -1.0f, 0, 0);
        else if (abs_hit.y() > abs_hit.z())
            local_normal = Eigen::Vector3f(0, (local_hit.y() > 0) ? 1.0f : -1.0f, 0);
        else
            local_normal = Eigen::Vector3f(0, 0, (local_hit.z() > 0) ? 1.0f : -1.0f);
    }

    // Transform normal to world space (rotation only)
    Eigen::Vector3f world_normal = (R * local_normal).normalized();

    // Convert hit point to world space
    Eigen::Vector3f world_hit = R * local_hit + _translation;

    // IMPORTANT: t_hit_local is in the same parametric units as ray.direction if rotation is orthonormal.
    // If your transform includes non-uniform scale, you must transform ray by scale too or compute world t separately.
    update_hit_from_intersection(
        hit,
        world_hit,
        world_normal,
        t_hit_local,
        this
    );

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

    // Local normal for a sphere
    Eigen::Vector3f local_normal = local_hit.normalized();

    // Transform hit back to world space
    Eigen::Matrix3f R = euler_to_matrix(_rotation);
    Eigen::Vector3f world_hit = R * (local_hit.cwiseProduct(_scale)) + _location;

    // Correct world normal (inverse-transpose method)
    Eigen::Vector3f world_normal =
        (R * (local_normal.cwiseProduct(inv_scale))).normalized();

    update_hit_from_intersection(
        hit,
        world_hit,
        world_normal,
        t0,
        this
    );

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
        update_hit_from_intersection(
            hit,
            ip,
            _normal,
            t,
            this
        );
        return true;
    }

    return false;
}
