/*
 *
 * This file will hold the code in order to parse the json file
 * from Render, and create and return the objects that it found.
 *
 * James Hocking, 2025 w. ChatGPT 5
 */

#pragma once

#include "Camera.h"
#include <string>

class BlenderFileReader {
private:
  Camera *get_camera_from_blender_file(std::string filepath);
};
