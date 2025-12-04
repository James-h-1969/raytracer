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
    // rotate to local space
    Eigen::Matrix3f R_inv = euler_to_matrix(_rotation).transpose();
    Eigen::Vector3f local_origin = R_inv * (ray.origin - _translation);
    Eigen::Vector3f local_direction = R_inv * ray.direction;

    // scale into unit
    Eigen::Vector3f inv_scale = _scale.cwiseInverse();
    local_origin = local_origin.cwiseProduct(inv_scale);
    local_direction = local_direction.cwiseProduct(inv_scale);

    const Eigen::Vector3f box_min(-0.5f, -0.5f, -0.5f);
    const Eigen::Vector3f box_max( 0.5f,  0.5f,  0.5f);

    float t_min_curr = -std::numeric_limits<float>::infinity();
    float t_max_curr = std::numeric_limits<float>::infinity();
    
    Eigen::Vector3f t_min_normal = Eigen::Vector3f::Zero(); 
    Eigen::Vector3f t_max_normal = Eigen::Vector3f::Zero();

    for (int i = 0; i < 3; ++i) {
        if (std::abs(local_direction[i]) < 1e-6f) {
            if (local_origin[i] < box_min[i] || local_origin[i] > box_max[i]) {
                return false;
            }
        } else {
            float inv_d = 1.0f / local_direction[i];
            float t0 = (box_min[i] - local_origin[i]) * inv_d;
            float t1 = (box_max[i] - local_origin[i]) * inv_d;
            
            float sign = (inv_d < 0.0f) ? 1.0f : -1.0f;
            Eigen::Vector3f current_normal = Eigen::Vector3f::Zero();
            current_normal[i] = 1.0f; 

            if (t0 > t1) {
                std::swap(t0, t1);
            }

            if (t0 > t_min_curr) {
                t_min_curr = t0;
                t_min_normal = current_normal * sign;
            }

            if (t1 < t_max_curr) {
                t_max_curr = t1;
                t_max_normal = current_normal * -sign; 
            }

            if (t_max_curr <= t_min_curr) return false;
        }
    }

    if (t_max_curr < 0) return false;

    float t_hit;
    Eigen::Vector3f local_normal;

    if (t_min_curr < 0) {
        t_hit = t_max_curr;
        local_normal = t_max_normal;
    } else {
        t_hit = t_min_curr;
        local_normal = t_min_normal; 
    }

    if (t_hit > 0) {
        Eigen::Vector3f local_hit = local_origin + (local_direction * t_hit);

        float u = 0.0f;
        float v = 0.0f;

        if (std::abs(local_normal.x()) > 0.5f) {
            u = local_hit.y() + 0.5f;
            v = local_hit.z() + 0.5f;
        } 
        else if (std::abs(local_normal.y()) > 0.5f) {
            u = 0.5f - local_hit.x();
            v = 0.5f - local_hit.z();
        } 
        else {
            u = local_hit.x() + 0.5f;
            v = local_hit.y() + 0.5f;
        }

        Eigen::Matrix3f R = euler_to_matrix(_rotation);
        Eigen::Vector3f world_hit = R * (local_hit.cwiseProduct(_scale)) + _translation;
        Eigen::Vector3f world_normal = (R * local_normal.cwiseProduct(inv_scale)).normalized();
        
        update_hit_from_intersection(hit, world_hit, world_normal, t_hit, this, u, v);
        
        return true;
    }

    return false;
}


bool Sphere::check_intersect(Ray& ray, Hit* hit) {
    // rotate into local space
    Eigen::Matrix3f R_inv = euler_to_matrix(_rotation).transpose();
    Eigen::Vector3f local_origin = R_inv * (ray.origin - _location);
    Eigen::Vector3f local_dir = R_inv * ray.direction;

    // turn ellipsoid into unit
    Eigen::Vector3f inv_scale = _scale.cwiseInverse();
    local_origin = local_origin.cwiseProduct(inv_scale);
    local_dir = local_dir.cwiseProduct(inv_scale);

    // ray intersect 
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

    // compute intersection point in local space
    Eigen::Vector3f local_hit = local_origin + t0 * local_dir;
    Eigen::Vector3f local_normal = local_hit.normalized();

    // transform hit back to world space
    Eigen::Matrix3f R = euler_to_matrix(_rotation);
    Eigen::Vector3f world_hit = R * (local_hit.cwiseProduct(_scale)) + _location;
    Eigen::Vector3f world_normal = (R * (local_normal.cwiseProduct(inv_scale))).normalized();

    float u = 0.5f + (std::atan2(local_hit.z(), local_hit.x()) / (2.0f * M_PI));
    float v = 0.5f - (std::asin(local_hit.y()) / M_PI);

    update_hit_from_intersection(
        hit,
        world_hit,
        world_normal,
        t0,
        this,
        u,
        v
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

    Eigen::Vector3f u_axis = _corners[2] - _corners[0];
    Eigen::Vector3f v_axis = _corners[3] - _corners[0];

    Eigen::Vector3f hit_vec = ip - _corners[0];

    float u = hit_vec.dot(u_axis) / u_axis.squaredNorm();
    float v = hit_vec.dot(v_axis) / v_axis.squaredNorm();

    if (x_check && y_check && z_check) {
        update_hit_from_intersection(
            hit,
            ip,
            _normal,
            t,
            this,
            u,
            v
        );
        return true;
    }

    return false;
}
