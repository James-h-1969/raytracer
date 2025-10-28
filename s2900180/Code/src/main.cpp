#include "BlenderFileReader.h"
#include "AccelerationHierarchy.h"

int main() {
  BlenderFileReader bfr = BlenderFileReader("../../ASCII/output_test.json");
  Camera c = bfr.get_camera_from_blender_file();
  std::vector<std::unique_ptr<Mesh>> meshes = bfr.get_meshes_from_blender_file();

  // create a acceleration hierarchy
  BoundingBoxHierarchyTree bbht = BoundingBoxHierarchyTree(std::move(meshes));

  return 0;
}
