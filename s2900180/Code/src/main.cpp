#include "BlenderFileReader.h"
#include "AccelerationHierarchy.h"
#include "Light.h"

int main() {
  BlenderFileReader bfr = BlenderFileReader("../../ASCII/sphere_skew.json");
  Camera c = bfr.get_camera_from_blender_file();
  CameraProperties props = c.get_camera_properties();
  std::vector<std::unique_ptr<Mesh>> meshes = bfr.get_meshes_from_blender_file();
  Light l = bfr.get_light_from_blender_file();

  // create a acceleration hierarchy
  BoundingBoxHierarchyTree bbht = BoundingBoxHierarchyTree(std::move(meshes));
  
  PPMImageFile image("");
  image.set_width_and_height(props.resolution_x, props.resolution_y);  

  Eigen::Vector3f base_colour(255.0f, 10.0f, 10.0f);
  
  int intersection_test_counter = 0;
  for (int px = 0; px < image.get_width(); px++) {
    for (int py = 0; py < image.get_height(); py++) {
      Pixel p = image.get_pixel(px, py);
      Ray r = p.as_ray(props);
      Hit h;

      if (bbht.check_intersect(r, &h, &intersection_test_counter)) {
        // shade according to blinn-phong
        Eigen::Vector3f s = shade(
          &h, 
          &l,
          &props,
          0.8f,
          0.2f, 
          20.0f,
          base_colour
        );
        image.update_pixel(px, py, s[0], s[1], s[2]);
      }
    }
  }

  image.write_current_image_to_file("../../Output/skew.ppm");


  return 0;
}
