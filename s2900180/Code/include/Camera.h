#pragma once

#include <string>

class Camera {
public:
  Camera();
  ~Camera();

private:
  void read_camera_info_from_json(std::string filename);
};
