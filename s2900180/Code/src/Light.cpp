#include "Light.h"

void update_hit_from_intersection(Hit *h, Eigen::Vector3f intersection_point,
                                  Eigen::Vector3f normal,
                                  float distance_along_ray, Mesh *mesh, float u,
                                  float v) {
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

Eigen::Vector3f shade(Hit *hit, std::vector<Light> lights,
                      CameraProperties *props, float Ia,
                      std::unique_ptr<BoundingBoxHierarchyTree> &bbht,
                      int depth, int max_depth) 
{
  Eigen::Vector3f P = hit->intersection_point;
  Eigen::Vector3f N = hit->normal.normalized();
  Eigen::Vector3f V = (P - props->location).normalized();
  Eigen::Vector3f shaded = Eigen::Vector3f::Zero();

  // convert to 0-1 colour space
  Eigen::Vector3f base_colour(hit->mesh->get_material().base_colour.r / 255.0f,
                              hit->mesh->get_material().base_colour.g / 255.0f,
                              hit->mesh->get_material().base_colour.b / 255.0f);

  // if material has a image texture, read base colour
  PPMImageFile *texture = hit->mesh->get_material().texture;
  if (texture != nullptr && hit->u >= 0 && hit->v >= 0 && hit->u <= 1 &&
      hit->v <= 1) {
    // Clamp coordinates to avoid segfaults at edges
    float u_coord = std::min(hit->u, 0.999f);
    float v_coord = std::min(hit->v, 0.999f);
    Pixel pixel = texture->get_pixel(u_coord * texture->get_width(),
                                     v_coord * texture->get_height());
    base_colour[0] *= pixel.colour.r / 255.0f;
    base_colour[1] *= pixel.colour.g / 255.0f;
    base_colour[2] *= pixel.colour.b / 255.0f;
  }

  float kd = hit->mesh->get_material().kd;
  float ks = hit->mesh->get_material().ks;
  float ka = hit->mesh->get_material().ka;
  float shininess = hit->mesh->get_material().shininess;
  float reflectivity = hit->mesh->get_material().reflectivity;
  float transparancy = hit->mesh->get_material().transparancy;
  float ior = hit->mesh->get_material().ior;
  int intersection_test_counter = 0;

  shaded += ka * Ia * base_colour; // ambiant

  for (Light &light : lights) {
    float id = light.get_id();
    float is = light.get_is();

    Eigen::Vector3f light_vec = light.get_location() - P;
    Eigen::Vector3f L = light_vec.normalized();
    float distance_to_light = light_vec.norm();
    float attenuation = std::min(1.0f / distance_to_light, 1.0f);

    Ray shadow_ray(P + (N * 0.001f), L);
    Hit shadow_hit;
    bool in_shadow = false;

    if (bbht->check_intersect(shadow_ray, &shadow_hit,
                              &intersection_test_counter)) {
      in_shadow = true;
    }

    if (!in_shadow) {
      float diff_factor = std::max(0.0f, N.dot(L));
      shaded += base_colour * kd * diff_factor * id * attenuation; // diffuse

      Eigen::Vector3f H = (L - V).normalized();
      float spec_factor = std::pow(std::max(0.0f, N.dot(H)), shininess);
      shaded += base_colour * ks * spec_factor * is * attenuation; // specular
    }
  }

  // reflection
  if (reflectivity > 0.0f && depth < max_depth) {
    Eigen::Vector3f R = (V - 2.0f * (N.dot(V)) * N).normalized();
    Ray reflect_ray(P + R * 0.001f, R);
    Hit reflect_hit;
    if (bbht->check_intersect(reflect_ray, &reflect_hit, &intersection_test_counter)) {
      Eigen::Vector3f reflected_colour = shade(&reflect_hit, lights, props, Ia, bbht, depth + 1, max_depth);
      shaded = shaded * (1.0f - reflectivity) + reflected_colour * reflectivity;
    }
  }

  return shaded;
}
