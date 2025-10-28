#include "Image.h"
#include "BlenderFileReader.h"
#include "Camera.h"
#include <chrono>
#include <iostream>
#include <Eigen/Dense>
#include <vector>

// Function that measures execution time of a void function taking the same parameters
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

// Example: simple dummy implementations
int hierarchy_acceleration(CameraProperties props, PPMImageFile& image, std::vector<std::unique_ptr<Mesh>>& meshes) {
  // Simulate some lightweight computation
  return 0;
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
          mesh->check_intersect(r, &h);
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

  // --- Set up test environment ---
  PPMImageFile image("");
  image.set_width_and_height(props.resolution_x, props.resolution_y);  
  std::vector<std::unique_ptr<Mesh>> meshes = bfr.get_meshes_from_blender_file(); 

  std::cout << "-- TESTING NO HIERARCHY --------------\n";
  measureExecutionTime(no_hierarchy_acceleration, props, image, meshes);
  std::cout << "--------------------------------------\n";

  std::cout << "\n-- TESTING HIERARCHY --------------\n";
  measureExecutionTime(hierarchy_acceleration, props, image, meshes);
  std::cout << "--------------------------------------\n";
}
