#include "BlenderFileReader.h"
#include "Image.h"
#include <Eigen/Dense>

int main() {
  BlenderFileReader bfr = BlenderFileReader("../../ASCII/output_test.json");
  Camera c = bfr.get_camera_from_blender_file();
  std::vector<std::unique_ptr<Mesh>> meshes =
      bfr.get_meshes_from_blender_file();

  // write to file
  std::ofstream out("../../Output/ray_intersect_test.txt");
  if (!out.is_open()) {
    std::cerr << "Failed to open for writing.\n";
    return 1;
  }

  for (int px = 0; px <= c.get_camera_properties().resolution_x;
       px += c.get_camera_properties().resolution_x / 20) {
    for (int py = 0; py <= c.get_camera_properties().resolution_y;
         py += c.get_camera_properties().resolution_y / 20) {
      Pixel p(px, py, 0, 0, 0);
      Ray r = p.as_ray(c.get_camera_properties());
      bool hit = false;
      std::string reason;
      Hit h;
      for (const auto &mesh : meshes) {
        if (mesh->get_meshtype() == MeshType::SPHERE) {
          if (mesh->check_intersect(r, &h)) {
            hit = true;
          }
        }
      }
      out << r.origin.x() << " " << r.origin.y() << " " << r.origin.z() << " "
          << r.direction.x() << " " << r.direction.y() << " "
          << r.direction.z();

      if (hit) {
        out << " g ";
        out << h.intersection_point[0] << " " << h.intersection_point[1] << " "
            << h.intersection_point[2] << "\n";
      } else {
        out << " r ";
        out << "0.0 0.0 0.0\n";
      }
    }
  }

  out.close();

  return 0;
}
