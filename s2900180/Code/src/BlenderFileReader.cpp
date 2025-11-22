#include "BlenderFileReader.h"

Camera BlenderFileReader::get_camera_from_blender_file() {
  // read the json file
  std::ifstream file(_filepath);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file at " << _filepath << std::endl;
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
  Eigen::Vector3f location = vec3_from_json(camera_json["location"]);
  Eigen::Vector3f gaze_vector = vec3_from_json(camera_json["gaze_vector_direction"]);
  Eigen::Vector3f up_vector = vec3_from_json(camera_json["up_vector"]);

  // classify the sensor fit (if AUTO, depends on which is larger between height and width)
  SensorFit sensor_fit = (camera_json["sensor_fit"] == "VERTICAL" || 
    (camera_json["sensor_fit"] == "AUTO" && camera_json["sensor_height"] > camera_json["sensor_width"]))
   ? SensorFit::VERTICAL : SensorFit::HORIZONTAL;

  float focal_length  = camera_json["focal_length"].get<float>();
  float sensor_width  = camera_json["sensor_width"].get<float>();
  float sensor_height = camera_json["sensor_height"].get<float>();
  float resolution_x  = camera_json["film_resolution"][0].get<float>();
  float resolution_y  = camera_json["film_resolution"][1].get<float>();

  CameraProperties camera_properties{location, gaze_vector, up_vector, sensor_fit, focal_length, sensor_width,
      sensor_height, resolution_x, resolution_y};

  Camera camera = Camera(camera_properties);
  return camera;
}

Material get_material_from_blender_object(const nlohmann::json& material_json) {
    Material material;
    material.ka = material_json["ka"];
    material.kd = vec3_from_json(material_json["kd"]);
    material.ks = vec3_from_json(material_json["ks"]);
    material.shininess = material_json["shininess"];
    material.base_colour = {material_json["base_color"][0], material_json["base_color"][1], material_json["base_color"][2]};
    material.reflectivity = material_json["reflectivity"];

    if (material_json.contains("texture")) { // only include texture if given
        std::string path = material_json["texture"]["absolute_path"];
        material.texture = new PPMImageFile(path);
        material.texture->read_image_from_file(); // TODO. add some caching here
    }
    return material;
}

std::vector<std::unique_ptr<Mesh>> BlenderFileReader::get_meshes_from_blender_file() {
  // read the json file
  std::ifstream file(_filepath);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file.\n";
    throw std::runtime_error("Could not open file");
  }

  nlohmann::json file_json; 
  file >> file_json;

  std::vector<std::unique_ptr<Mesh>> meshes;

  for (const auto& object : file_json["objects"]) {
      if (object["type"] != "MESH") continue;

      std::string shape = object["shape"];
      std::string name = object["name"];

      if (shape == "CUBE") {
          Eigen::Vector3f translation = vec3_from_json(object["translation"]);
          Eigen::Vector3f rotation = vec3_from_json(object["rotation_euler_rad"]);
          Eigen::Vector3f scale = vec3_from_json(object["scale"]);
          Material material = get_material_from_blender_object(object["material"]);
          meshes.push_back(std::make_unique<Cube>(translation, rotation, scale, name, MeshType::CUBE, material));
      } 
      else if (shape == "SPHERE") {
          Eigen::Vector3f location = vec3_from_json(object["location"]); // TODO. change naming convention
          Eigen::Vector3f rotation = vec3_from_json(object["rotation_euler_rad"]);
          Eigen::Vector3f scale = vec3_from_json(object["scale"]);
          Material material = get_material_from_blender_object(object["material"]);
          meshes.push_back(std::make_unique<Sphere>(location, rotation, scale, name, MeshType::SPHERE, material));
      } 
      else if (shape == "PLANE") {
          std::array<Eigen::Vector3f, NUMBER_OF_PLANE_CORNERS> corners;
          for (int i = 0; i < NUMBER_OF_PLANE_CORNERS; ++i) {
              corners[i] = vec3_from_json(object["corners_world"][i]);
          }
          Material material = get_material_from_blender_object(object["material"]);
          meshes.push_back(std::make_unique<Plane>(corners, name, MeshType::PLANE, material));
      }
  }
  return meshes;
}

std::vector<Light> BlenderFileReader::get_lights_from_blender_file() {
  // read the json file
  std::ifstream file(_filepath);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file.\n";
    throw std::runtime_error("Could not open file");
  }

  nlohmann::json file_json; 
  file >> file_json;

  std::vector<Light> l;
  for (const auto& object : file_json["objects"]) {
    if (object["type"] == "LIGHT") {
        Eigen::Vector3f position = vec3_from_json(object["location"]); // TODO. change naming convention here
        Light curr_l(position, object["radiant_intensity"]);
        l.push_back(curr_l);
    }
  }

  return l;
};
