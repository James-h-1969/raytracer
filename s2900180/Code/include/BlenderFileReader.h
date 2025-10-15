/*
BlenderFileReader.h
James Hocking, 2025
*/

#pragma once

#include "Camera.h"
#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class BlenderFileReader {
  /*
  This class will be directly connected to a file exported from blender
  and will be used in order to process and create the required objects from
  the file.
  */
  public: 
    BlenderFileReader(std::string filepath) : _filepath(filepath) {
      std::cout << "Created a blender file reader for " << _filepath << "\n\n";
    };

    // function that reads the JSON of the blender file and creates the Camera object
    Camera& get_camera_from_blender_file();

    // function that reads the JSON of the blender file and creates a vector of Mesh pointers
    std::vector<Mesh*> get_meshes_from_blender_file();

  private:
    // blender file to read, likely found in ../../ASCII/file.json
    std::string _filepath;
};
