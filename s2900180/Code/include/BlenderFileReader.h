/*
BlenderFileReader.h
James Hocking, 2025
*/

#pragma once

#include "Camera.h"
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

class BlenderFileReader {
  /*
  This class will be directly connected to a file exported from blender
  and will be used in order to process and create the required objects from
  the file.
  */
  public: 
    BlenderFileReader(std::string filepath) : _filepath(filepath), _camera_read(false) {
      std::cout << "Created a blender file reader for " << _filepath << std::endl;
    };
    Camera& get_camera_from_blender_file();

  private:
    std::string _filepath;
    Camera _camera;
    bool _camera_read;
};
