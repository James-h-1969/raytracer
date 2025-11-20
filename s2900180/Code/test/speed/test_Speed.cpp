#include "Image.h"
#include "BlenderFileReader.h"
#include "AccelerationHierarchy.h"
#include "Camera.h"
#include <chrono>
#include <iostream>
#include <Eigen/Dense>
#include <vector>

struct Colour white{255, 255, 255};

void measureExecutionTime(
    int (*func)(CameraProperties, PPMImageFile&, std::vector<std::unique_ptr<Mesh>>&),
    CameraProperties props, PPMImageFile& image, std::vector<std::unique_ptr<Mesh>>& meshes) {

  auto start = std::chrono::high_resolution_clock::now();

  int cycle_count = func(props, image, meshes); // call the function with parameters

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
  std::cout << "Amount of intersection tests: " << cycle_count << std::endl;
}

int hierarchy_acceleration(CameraProperties props, PPMImageFile& image, std::vector<std::unique_ptr<Mesh>>& meshes) {
    BoundingBoxHierarchyTree bbht = BoundingBoxHierarchyTree(std::move(meshes));
    int counter = 0;
    for (int px = 0; px < image.get_width(); px++) {
      for (int py = 0; py < image.get_height(); py++) {
        Pixel p = image.get_pixel(px, py);
        Ray r = p.as_ray(props);
        Hit h;
        if (bbht.check_intersect(r, &h, &counter)) {
          image.update_pixel(px, py, white);
        }
      }
    }
  return counter;
}

int no_hierarchy_acceleration(CameraProperties props, PPMImageFile& image, std::vector<std::unique_ptr<Mesh>>& meshes) {
  /*
    This is the brute-force method:
    Go through every single ray and query against every single mesh
  */
  int counter = 0;
  for (int px = 0; px < image.get_width(); px++) {
    for (int py = 0; py < image.get_height(); py++) {
      Pixel p = image.get_pixel(px, py);
      Ray r = p.as_ray(props);
      for (auto& mesh : meshes) {
          Hit h;
          if (mesh->check_intersect(r, &h)) {
            image.update_pixel(px, py, white);
          };
          counter++;
      }
    }
  }
  return counter;
}

int main() {
  std::string filepath = std::string(TEST_DATA_DIR) + "/intersect_test.json";
  BlenderFileReader bfr = BlenderFileReader(filepath); 
  Camera c = bfr.get_camera_from_blender_file();
  CameraProperties props = c.get_camera_properties();

  PPMImageFile image("");
  image.set_width_and_height(props.resolution_x, props.resolution_y);  
  std::vector<std::unique_ptr<Mesh>> meshes = bfr.get_meshes_from_blender_file(); 

  std::cout << "-- TESTING NO HIERARCHY --------------\n";
  measureExecutionTime(no_hierarchy_acceleration, props, image, meshes);
  std::string image_filepath = std::string(TEST_DATA_DIR) + "/image_result_no_hierarchy.ppm";
  image.write_current_image_to_file(image_filepath);
  std::cout << "--------------------------------------\n";

  std::cout << "\n-- TESTING HIERARCHY --------------\n";
  measureExecutionTime(hierarchy_acceleration, props, image, meshes);
  std::string image_filepath_h = std::string(TEST_DATA_DIR) + "/image_result_hierarchy.ppm";
  image.write_current_image_to_file(image_filepath_h);
  std::cout << "--------------------------------------\n";
}
