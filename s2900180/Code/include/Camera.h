/*
Camera.h
James Hocking, 2025
*/

#pragma once

#include <string>
#include <Eigen/Dense>

struct CameraProperties {
  /*
  This struct holds the main quantative properties of the 
  camera that is being simulated.
  */
    Eigen::Vector3f location;
    Eigen::Vector3f gaze_vector_direction;
    float focal_length;
    float sensor_width;
    float sensor_height;
    float resolution_x;
    float resolution_y;
};

class Camera {
  /*
  This class will hold the properties of the camera and the main 
  functions required to simulated its use.  
  */
  public:
    Camera(struct CameraProperties camera_properties) : _camera_properties(camera_properties) {};

  private:
    struct CameraProperties _camera_properties;
};
