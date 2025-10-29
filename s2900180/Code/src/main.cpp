#include "BlenderFileReader.h"
#include "AccelerationHierarchy.h"

int main() {
  // read the blender file
  BlenderFileReader bfr = BlenderFileReader("../../ASCII/sphere_skew.json");
  Camera c = bfr.get_camera_from_blender_file();
  CameraProperties props = c.get_camera_properties();
  std::vector<std::unique_ptr<Mesh>> meshes = bfr.get_meshes_from_blender_file();

  // create a acceleration hierarchy
  BoundingBoxHierarchyTree bbht = BoundingBoxHierarchyTree(std::move(meshes));
  PPMImageFile image("");
  image.set_width_and_height(props.resolution_x, props.resolution_y);  
  
  int counter = 0;
  for (int px = 0; px < image.get_width(); px++) {
    for (int py = 0; py < image.get_height(); py++) {
      Pixel p = image.get_pixel(px, py);
      Ray r = p.as_ray(props);
      Hit h;
      if (bbht.check_intersect(r, &h, &counter)) {
        image.update_pixel(px, py, 255, 255, 255);
      }
    }
  }

  image.write_current_image_to_file("../../Output/skew.ppm");

  return 0;
}
