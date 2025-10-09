/*
BlenderFileReader.h
James Hocking, 2025
*/

#pragma once

#include "Camera.h"
#include <iostream>
#include <string>

class BlenderFileReader {
  /*
  This class will be directly connected to a file exported from blender
  and will be used in order to process and create the required objects from
  the file.
  */
  public: 
    BlenderFileReader(std::string filepath) : _filepath(filepath) {
      std::cout << "Created a blender file reader for " << _filepath << std::endl;
    };

  private:
    std::string _filepath;
    Camera& get_camera_from_blender_file();
};
