#include "BlenderFileReader.h"

Camera& BlenderFileReader::get_camera_from_blender_file() {
  // read the json file
  std::ifstream file(_filepath);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file.\n";
    throw std::runtime_error("Could not open file");
  }

  nlohmann::json file_json; 
  file >> file_json;

  // seperate relevant camera json
  nlohmann::json camera_json; 
  for (const auto& object : file_json["objects"]) {
    if (object["type"] == "camera") {
      camera_json = object;
    }
  }
  if (camera_json.is_null()) {
    throw std::runtime_error("No camera object found in file");
  }

  Eigen::Vector3f location = {
        camera_json["location"][0].get<float>(),
        camera_json["location"][1].get<float>(),
        camera_json["location"][2].get<float>()
    };

    Eigen::Vector3f gaze_vector = {
        camera_json["gaze_vector_direction"][0].get<float>(),
        camera_json["gaze_vector_direction"][1].get<float>(),
        camera_json["gaze_vector_direction"][2].get<float>()
    };

    float focal_length  = camera_json["focal_length"].get<float>();
    float sensor_width  = camera_json["sensor_width"].get<float>();
    float sensor_height = camera_json["sensor_height"].get<float>();
    float resolution_x  = camera_json["resolution_x"].get<float>();
    float resolution_y  = camera_json["resolution_y"].get<float>();

    // Construct CameraProperties
    CameraProperties camera_properties{
        location,
        gaze_vector,
        focal_length,
        sensor_width,
        sensor_height,
        resolution_x,
        resolution_y
    };

    // Store the camera as a member variable (so the reference is valid)
    _camera = Camera(camera_properties);
    return _camera;

}
