#include "Raytracer.h"

void RayTracer::setup(std::string filename) {
    BlenderFileReader bfr(filename);
    Camera c = bfr.get_camera_from_blender_file();
    props = c.get_camera_properties();
    std::vector<std::unique_ptr<Mesh>> meshes = bfr.get_meshes_from_blender_file();
    lights = bfr.get_lights_from_blender_file();
    bbht = std::make_unique<BoundingBoxHierarchyTree>(std::move(meshes));
}

void RayTracer::render_image(std::string output_filename) {
    if (!bbht) { // setup not complete
        return; 
    }

    PPMImageFile image("");
    image.set_width_and_height(props.resolution_x, props.resolution_y);
    const int amount_of_antialiasing_samples_per_pixel = 4;
    Eigen::Vector3f base_colour(255.0f, 10.0f, 10.0f);

    std::mt19937 generator(0); 
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    int intersection_test_counter = 0;
    for (int px = 0; px < image.get_width(); px++) {
        for (int py = 0; py < image.get_height(); py++) {
            Eigen::Vector3f overall_shade = Eigen::Vector3f::Zero();

            bool is_hit = false;
            for (int sample_i = 0; sample_i < amount_of_antialiasing_samples_per_pixel; sample_i++) {
                float sample_offset_x = distribution(generator);
                float sample_offset_y = distribution(generator);

                float sample_px = std::min((float)px + sample_offset_x, (float)image.get_width()-1.0f);
                float sample_py = std::min((float)py + sample_offset_y, (float)image.get_height()-1.0f);

                Pixel p = image.get_pixel(sample_px, sample_py);
                Ray r = p.as_ray(props);
                Hit h;

                if (bbht->check_intersect(r, &h, &intersection_test_counter)) {
                    Eigen::Vector3f s = shade(&h, lights, &props, 1.0f, bbht, 0);
                    overall_shade[0] += s[0] * 255.0f;
                    overall_shade[1] += s[1] * 255.0f;
                    overall_shade[2] += s[2] * 255.0f;
                    is_hit = true;
                }
            }
            overall_shade /= amount_of_antialiasing_samples_per_pixel; // finding the average
            if (!is_hit) overall_shade += blender_background;
            Colour new_colour{overall_shade[0], overall_shade[1], overall_shade[2]};
            
            image.update_pixel(px, py, new_colour);
        }
    }

    image.write_current_image_to_file(output_filename);
}