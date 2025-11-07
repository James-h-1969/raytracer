#include "Light.h"

void update_hit_from_intersection(Hit* h, Eigen::Vector3f intersection_point, Eigen::Vector3f normal, float distance_along_ray) {
    // update only if not hit yet or closer then best hit so far
    if (!h->is_hit || (h->is_hit && distance_along_ray < h->distance_along_ray)) {
        h->normal = normal;
        h->distance_along_ray = distance_along_ray;
        h->is_hit = true;
        h->intersection_point = intersection_point;
    } 
};

Eigen::Vector3f shade(Hit* hit, Light* light, CameraProperties* props, float kd, float ks, float shininess, Eigen::Vector3f base_colour) {
    // for the sake of the formula, get the vectors required
    Eigen::Vector3f P = hit->intersection_point;
    Eigen::Vector3f N = hit->normal.normalized();
    Eigen::Vector3f L = (light->get_location() - P).normalized();
    Eigen::Vector3f V = (props->location - P).normalized();
    Eigen::Vector3f H = (L+V).normalized();

    float r2 = (light->get_location() - P).squaredNorm();
    float E = light->get_radiant_intensity() / r2;

    float diff = std::max(0.0f, N.dot(L));
    Eigen::Vector3f diffuse = kd * E * diff * base_colour;

    float spec = std::pow(std::max(0.0f, N.dot(H)), shininess);
    Eigen::Vector3f white_light(0.0f, 0.0f, 0.0f);
    Eigen::Vector3f specular = ks * E * spec * white_light;

    return diffuse + specular;
}