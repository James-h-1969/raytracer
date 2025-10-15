#include "BlenderFileReader.h"
#include "Image.h"
#include <iostream>

#define FILENAME std::string("../../ASCII/output_test.json")

int main() {
    BlenderFileReader blender_file_reader = BlenderFileReader(FILENAME);
    Camera& camera = blender_file_reader.get_camera_from_blender_file();
    camera.get_camera_properties().print();

    generate_ray_text_file(camera);

    // PPMImageFile ppm_image = PPMImageFile("../../Output/test.ppm");
    
    return 0;
}
