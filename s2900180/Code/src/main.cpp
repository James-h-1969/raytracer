#include "BlenderFileReader.h"
#include "Image.h"

int main() {
    BlenderFileReader bfr = BlenderFileReader("../../ASCII/output_test.json");
    Camera c = bfr.get_camera_from_blender_file();
    int amount_of_rays = 10;
    generate_ray_text_file(c, "../../Output/ray_test.txt", amount_of_rays);
    return 0;
}
