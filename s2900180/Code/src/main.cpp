#include "BlenderFileReader.h"
#include <iostream>

#define FILENAME std::string("ASCII/test.json")

int main() {
    std::cout << " -- TESTING BLENDER FILE READER --" << std::endl;

    BlenderFileReader blender_file_reader = BlenderFileReader(FILENAME);
    Camera& camera = blender_file_reader.get_camera_from_blender_file();
    camera.get_camera_properties().print();

    std::vector<Mesh> meshes = blender_file_reader.get_meshes_from_blender_file();
    for (Mesh& mesh: meshes) {
        mesh.show_properties();
    }

    return 0;
}
