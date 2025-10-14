#include "BlenderFileReader.h"
#include "Image.h"
#include <iostream>

#define FILENAME std::string("../../ASCII/test.json")

int main() {
    // PART 1
    std::cout << " -- TESTING BLENDER FILE READER --" << std::endl;

    BlenderFileReader blender_file_reader = BlenderFileReader(FILENAME);
    Camera& camera = blender_file_reader.get_camera_from_blender_file();
    camera.get_camera_properties().print();

    std::vector<Mesh*> meshes = blender_file_reader.get_meshes_from_blender_file();
    for (Mesh* mesh: meshes) {
        mesh->show_properties();
    }

    // PART 2 - RAYS
    generate_ray_text_file(camera);

    return 0;
}
