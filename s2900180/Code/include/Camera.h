/*
Camera.h
James Hocking, 2025
*/

#pragma once

#include <string>
#include <iostream>
#include <Eigen/Dense>

enum class SensorFit {
  /*
  Enum that represents the two types of sensor fits. In vertical mode, 
  the height is the sensor height and then the width is varied to match the 
  aspect ratio. In horizontal mode, this is flipped. Blender defaults to 
  AUTO, in which the BlenderFileReader class determines as vertical
  or horizontal based on the size of the sensor height and width.
  */
  VERTICAL,
  HORIZONTAL,
};

struct CameraProperties {
  /*
  This struct holds the main quantative properties of the 
  camera that is being simulated.
  */
    Eigen::Vector3f location;
    Eigen::Vector3f gaze_vector_direction;
    Eigen::Vector3f up_vector;
    enum SensorFit sensor_fit;
    float focal_length;
    float sensor_width;
    float sensor_height;
    float resolution_x;
    float resolution_y;

    void print() const {
        std::cout << "Camera Properties:\n";
        std::cout << "  Location: [" << location.x() << ", " << location.y() << ", " << location.z() << "]\n";
        std::cout << "  Gaze Vector: [" << gaze_vector_direction.x() << ", "
                  << gaze_vector_direction.y() << ", "
                  << gaze_vector_direction.z() << "]\n";
        std::cout << "  Up Vector: [" << up_vector.x() << ", "
                  << up_vector.y() << ", "
                  << up_vector.z() << "]\n";
        std::cout << "  Sensor fit: " << (sensor_fit == SensorFit::VERTICAL ? "Vertical" : "Horizontal") << "\n";
        std::cout << "  Focal Length: " << focal_length << " mm\n";
        std::cout << "  Sensor Size: " << sensor_width << " x " << sensor_height << " mm\n";
        std::cout << "  Resolution: " << resolution_x << " x " << resolution_y << " px\n";
    }
};

class Camera {
  /*
  This class will hold the properties of the camera and the main 
  functions required to simulated its use.  
  */
  public:
    Camera(struct CameraProperties camera_properties) : _camera_properties(camera_properties) {};

    struct CameraProperties get_camera_properties() { return _camera_properties; };

  private:
    struct CameraProperties _camera_properties;
};
