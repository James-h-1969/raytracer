#pragma once

#include <string>
#include <memory> 
#include "Image.h"
#include "BlenderFileReader.h"
#include "Camera.h"
#include "Mesh.h"
#include <algorithm>
#include <random>
#include "Light.h"
#include "AccelerationHierarchy.h"

const Eigen::Vector3f blender_background = {70.0f, 70.0f, 70.0f};
const int amount_of_antialiasing_samples_per_pixel = 4; // make this a setting

struct RayTracerSettings 
{
    std::string input_filename = std::string("No input filename detected. use --input flag");
    std::string output_filename = std::string("No output filename detected. use --output flag");;

    // TODO. more custom settings here
};

class RayTracer 
{
    public:
        RayTracer() : _bbht(nullptr) {}

        /*
        This is a function that turns the command line into a tool that can change the 
        settings used in the raytracer.
        */
        void create_settings_from_command_args(int argc, char* argv[]);
        
        /*
        This function reads all the files and creates all the objects that is needed for the 
        raytracer to run.
        */
        void setup();

        /*
        Function that iterates through every pixel in an image, creates a ray and projects it 
        onto the scene.
        */
        void render_image();

    private:
        CameraProperties _props;
        RayTracerSettings _ray_tracer_settings;
        std::vector<Light> _lights;
        std::unique_ptr<BoundingBoxHierarchyTree> _bbht;
};