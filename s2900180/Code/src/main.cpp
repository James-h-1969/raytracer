#include "BlenderFileReader.h"
#include "Image.h"
#include <Eigen/Dense>

int main() {
  BlenderFileReader bfr = BlenderFileReader("../../ASCII/output_test.json");
  Camera c = bfr.get_camera_from_blender_file();
  std::vector<std::unique_ptr<Mesh>> meshes =
      bfr.get_meshes_from_blender_file();

  out.close();

  return 0;
}
