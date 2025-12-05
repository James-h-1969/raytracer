#include "Raytracer.h"

void RayTracer::create_settings_from_command_args(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        char* current_setting = argv[i];
        if (!strcmp(current_setting, "--input")) {
            _ray_tracer_settings.input_filename =  argv[i+1];
        } else if (!strcmp(current_setting, "--output")) {
            _ray_tracer_settings.output_filename = argv[i+1];
        } else if (!strcmp(current_setting, "--antialiasing")) {
            _ray_tracer_settings.amount_of_antialiasing_samples_per_pixel = atoi(argv[i+1]); 
        } else if (!strcmp(current_setting, "--recursion-depth")) {
            _ray_tracer_settings.max_depth_of_reflection_recursion = atoi(argv[i+1]);
        } // TODO. added distributed rt, lens effects
    }
}

void RayTracer::setup() {
    BlenderFileReader bfr(_ray_tracer_settings.input_filename);

    // read the blender file
    Camera camera =                             bfr.get_camera_from_blender_file();
    std::vector<std::unique_ptr<Mesh>> meshes = bfr.get_meshes_from_blender_file();
    _lights =                                   bfr.get_lights_from_blender_file();

    _props = camera.get_camera_properties();
    _bbht = std::make_unique<BoundingBoxHierarchyTree>(std::move(meshes));
}

void RayTracer::render_image() {
    std::cout << "Rendering image, please be patient...\n";
    
    PPMImageFile image("");
    image.set_width_and_height(_props.resolution_x, _props.resolution_y);

    // create uniform distribution for antialiasing sampling
    std::mt19937 generator(0); 
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    int intersection_test_counter = 0;
    // iterate through each pixel, shade correspondingly
    for (int px = 0; px < image.get_width(); px++) {
        for (int py = 0; py < image.get_height(); py++) {            
            Eigen::Vector3f overall_shade = Eigen::Vector3f::Zero();
            bool is_hit = false;
            for (int sample_i = 0; sample_i < _ray_tracer_settings.amount_of_antialiasing_samples_per_pixel; sample_i++) {
                float sample_offset_x = distribution(generator);
                float sample_offset_y = distribution(generator);

                // lock into bounds of image
                float sample_px = std::min((float)px + sample_offset_x, (float)image.get_width()-1.0f);
                float sample_py = std::min((float)py + sample_offset_y, (float)image.get_height()-1.0f);

                Pixel p = image.get_pixel(sample_px, sample_py);
                Ray r = p.as_ray(_props);
                Hit h;

                if (_bbht->check_intersect(r, &h, &intersection_test_counter)) {
                    Eigen::Vector3f s = shade(&h, _lights, &_props, 1.0f, _bbht, 0, _ray_tracer_settings.max_depth_of_reflection_recursion);
                    
                    // shade operates in 0-1 shading region, convert back to rgb255
                    overall_shade[0] += s[0] * 255.0f;
                    overall_shade[1] += s[1] * 255.0f;
                    overall_shade[2] += s[2] * 255.0f;

                    is_hit = true;
                }
            }

            overall_shade /= _ray_tracer_settings.amount_of_antialiasing_samples_per_pixel; // finding the average
            if (!is_hit) overall_shade += blender_background;
            Colour new_colour{overall_shade[0], overall_shade[1], overall_shade[2]};
            
            image.update_pixel(px, py, new_colour);
        }
    }

    image.write_current_image_to_file(_ray_tracer_settings.output_filename);
}