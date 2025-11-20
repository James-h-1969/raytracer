#pragma once

#include <string>
#include <memory> // Required for std::unique_ptr
#include "Image.h"
#include "BlenderFileReader.h"
#include "Camera.h"
#include "Mesh.h"
#include <algorithm>
#include <random>
#include "Light.h"
#include "AccelerationHierarchy.h"

const Eigen::Vector3f blender_background = {70.0f, 70.0f,70.0f};

class RayTracer {
public:
    // Constructor to initialize pointer
    RayTracer() : bbht(nullptr) {}

    void setup(std::string filename);
    void render_image(std::string output_filename);

private:
    CameraProperties props;
    std::vector<Light> lights;
    std::unique_ptr<BoundingBoxHierarchyTree> bbht;
};