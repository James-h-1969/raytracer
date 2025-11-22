#include "Light.h"

void update_hit_from_intersection(Hit* h, Eigen::Vector3f intersection_point, Eigen::Vector3f normal, float distance_along_ray, Mesh* mesh, float u, float v) {
    // update only if not hit yet or closer then best hit so far
    if (!h->is_hit || (h->is_hit && distance_along_ray < h->distance_along_ray)) {
        h->normal = normal;
        h->distance_along_ray = distance_along_ray;
        h->is_hit = true;
        h->intersection_point = intersection_point;
        h->mesh = mesh;
        h->u = u;
        h->v = v;
    } 
};

Eigen::Vector3f shade(Hit* hit, std::vector<Light> lights, CameraProperties* props, float Ia, std::unique_ptr<BoundingBoxHierarchyTree>& bbht, int depth) {
    const int MAX_DEPTH = 5; // TODO. make this configurable from the command line 

    // TODO. change anything that should be a Colour class to colour

    Eigen::Vector3f P = hit->intersection_point;
    Eigen::Vector3f N = hit->normal.normalized();
    Eigen::Vector3f V = (P - props->location).normalized();

    Eigen::Vector3f shaded = Eigen::Vector3f::Zero();

    Eigen::Vector3f base_colour(
        hit->mesh->get_material().base_colour.r / 255.0f,
        hit->mesh->get_material().base_colour.g / 255.0f,
        hit->mesh->get_material().base_colour.b / 255.0f
    );

    // if texture, override base colour 
    PPMImageFile* texture = hit->mesh->get_material().texture;
    if (texture != nullptr && hit->u >= 0 && hit->v >= 0 & hit->u < 1 && hit->v < 1) {
        Pixel pixel = texture->get_pixel(hit->u*texture->get_width(), hit->v*texture->get_height());
        base_colour[0] *= pixel.colour.r / 255.0f;
        base_colour[1] *= pixel.colour.g / 255.0f;
        base_colour[2] *= pixel.colour.b / 255.0f;
    }

    Eigen::Vector3f kd = hit->mesh->get_material().kd;
    Eigen::Vector3f ks = hit->mesh->get_material().ks;
    float ka = hit->mesh->get_material().ka;          
    float shininess = hit->mesh->get_material().shininess;
    float reflectivity = hit->mesh->get_material().reflectivity;

    int intersection_test_counter = 0;

    // Ambient
    shaded += ka * Ia * base_colour;

    // Direct Lighting
    for (Light light : lights) {
        Eigen::Vector3f light_vec = light.get_location() - P;
        float r2 = light_vec.squaredNorm();
        Eigen::Vector3f L = light_vec.normalized();

        // Shadow ray
        Ray shadow_ray(P + L * 0.001f, L);
        Hit shadow_hit;
        if (bbht->check_intersect(shadow_ray, &shadow_hit, &intersection_test_counter)) {
            continue;
        }

        Eigen::Vector3f H = (L + V).normalized();

        // Diffuse
        float diff = std::max(0.0f, N.dot(L));
        shaded += kd * diff;

        // Specular
        float spec = std::pow(std::max(0.0f, V.dot(H)), shininess);
        shaded += ks * spec;
    }

    // reflection
    if (reflectivity > 0.0f && depth < MAX_DEPTH) {
        Eigen::Vector3f R = (V - 2.0f * (V.dot(N)) * N).normalized();
        Ray reflect_ray(P + R * 0.001f, R);

        Hit reflect_hit;
        if (bbht->check_intersect(reflect_ray, &reflect_hit, &intersection_test_counter)) {
            Eigen::Vector3f reflected = shade(&reflect_hit, lights, props, Ia, bbht, depth + 1);
            shaded = shaded * (1.0f - reflectivity) + reflected * reflectivity;
        }
    }


    return shaded;   
}
