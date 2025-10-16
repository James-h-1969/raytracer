#include "BlenderFileReader.h"

Camera BlenderFileReader::get_camera_from_blender_file() {
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
    if (object["type"] == "CAMERA") {
      camera_json = object;
    }
  }
  if (camera_json.is_null()) {
    throw std::runtime_error("No camera object found in file");
  }

  // construct camera object
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

  Eigen::Vector3f up_vector = {
    camera_json["up_vector"][0].get<float>(),
    camera_json["up_vector"][1].get<float>(),
    camera_json["up_vector"][2].get<float>(),
  };

  // classify the sensor fit (if AUTO, depends on which is larger between height and width)
  SensorFit sensor_fit = (camera_json["sensor_fit"] == "VERTICAL" || 
    (camera_json["sensor_fit"] == "AUTO" && camera_json["sensor_height"] > camera_json["sensor_width"]))
   ? SensorFit::VERTICAL : SensorFit::HORIZONTAL;

  float focal_length  = camera_json["focal_length"].get<float>();
  float sensor_width  = camera_json["sensor_width"].get<float>();
  float sensor_height = camera_json["sensor_height"].get<float>();
  float resolution_x  = camera_json["film_resolution"][0].get<float>();
  float resolution_y  = camera_json["film_resolution"][1].get<float>();

  CameraProperties camera_properties{
      location,
      gaze_vector,
      up_vector,
      sensor_fit,
      focal_length,
      sensor_width,
      sensor_height,
      resolution_x,
      resolution_y
  };

  Camera camera = Camera(camera_properties);
  return camera;
}

std::vector<Mesh*> BlenderFileReader::get_meshes_from_blender_file() {
  // read the json file
  std::ifstream file(_filepath);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file.\n";
    throw std::runtime_error("Could not open file");
  }

  nlohmann::json file_json; 
  file >> file_json;

  std::vector<Mesh*> meshes;

  for (const auto& object : file_json["objects"]) {
      // Skip non-mesh types (like LIGHT, CAMERA, etc.)
      if (object["type"] != "MESH") continue;

      std::string shape = object["shape"];
      std::string name = object["name"];

      if (shape == "CUBE") {
          Eigen::Vector3f translation(
              object["translation"][0],
              object["translation"][1],
              object["translation"][2]
          );
          Eigen::Vector3f rotation(
              object["rotation_euler_rad"][0],
              object["rotation_euler_rad"][1],
              object["rotation_euler_rad"][2]
          );
          float scale = object["scale_1d"];

          Cube* mesh = new Cube(translation, rotation, scale, name, MeshType::CUBE);
          meshes.push_back(mesh);
      } 
      else if (shape == "SPHERE") {
          Eigen::Vector3f location(
              object["location"][0],
              object["location"][1],
              object["location"][2]
          );
          float radius = object["radius"];

          Sphere* mesh = new Sphere(location, radius, name, MeshType::SPHERE);
          meshes.push_back(mesh);
      } 
      else if (shape == "PLANE") {
          std::array<Eigen::Vector3f, 4> corners;

          for (int i = 0; i < 4; ++i) {
              corners[i] = Eigen::Vector3f(
                  object["corners_world"][i][0],
                  object["corners_world"][i][1],
                  object["corners_world"][i][2]
              );
          }

          Plane* mesh = new Plane(corners, name, MeshType::PLANE);
          meshes.push_back(mesh);
      }
  }
  return meshes;
}
