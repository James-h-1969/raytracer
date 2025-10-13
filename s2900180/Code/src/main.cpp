#include "BlenderFileReader.h"

#define FILENAME std::string("ASCII/test.json")

int main() {
    BlenderFileReader blender_file_reader = BlenderFileReader(FILENAME);
    Camera& camera = blender_file_reader.get_camera_from_blender_file();
    camera.get_camera_properties().print();
    return 0;
}
