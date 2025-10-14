#include "Image.h"

void PPMImageFile::read_image_from_file() {
    // open the file 
    std::ifstream file(_filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file.\n";
        throw std::runtime_error("Could not open file");
    }

    int width, height;
    file >> width >> height;

    std::string format;
    file >> format;
    if (format != "P3") {
        std::cerr << "Error: Unsupported PPM format (expected P3, got " << format << ")" << std::endl;
        return;
    }

    _image_map.clear();

    for (int i = 0; i < height; i++) {
        std::vector<Pixel> row = {};
        for (int j = 0; j < width; j++) {
            int r, g, b;
            file >> r >> g >> b;
            
            // create the pixel from the row
            Pixel p;
            p.px = j;
            p.py = i;
            p.r = r;
            p.g = g;
            p.b = b;
            row.push_back(p);
        }
        _image_map.push_back(row);
    }
}

void PPMImageFile::update_pixel(int px, int py, int r, int g, int b) {
    Pixel to_update = _image_map.at(py).at(px);
    to_update.r = r;
    to_update.g = g;
    to_update.b = b;
}

void PPMImageFile::write_current_image_to_file(std::string export_filename) {
    std::ofstream out(export_filename);
    if (!out) {
        std::cerr << "Error: Could not open file for writing\n";
        return;
    }

    // header
    out << "P3\n" << _image_map.at(0).size() << " " << _image_map.size() << "\n255\n";

    for (std::vector<Pixel> row: _image_map) {
        for (Pixel p: row) {
            out << p.r << " " << p.g << " " << p.b << "  ";
        }
        out << "\n";
    }

    out.close();    
}

void generate_ray_text_file(Camera& camera) {
    const auto& props = camera.get_camera_properties();
    std::vector<Ray> rays;

    // Camera basis
    Eigen::Vector3f forward = props.gaze_vector_direction.normalized();
    Eigen::Vector3f world_up(0, 0, 1);
    Eigen::Vector3f right = forward.cross(world_up).normalized();
    Eigen::Vector3f up = right.cross(forward).normalized();

    // Image plane
    Eigen::Vector3f image_center = props.location + forward * props.focal_length;
    float pixel_width = props.sensor_width / props.resolution_x;
    float pixel_height = props.sensor_height / props.resolution_y;
    Eigen::Vector3f top_left = image_center - right * (props.sensor_width / 2.0f) + up * (props.sensor_height / 2.0f);

    // Random sampling for 50 rays
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist_x(0, props.resolution_x - 1);
    std::uniform_int_distribution<int> dist_y(0, props.resolution_y - 1);

    for (int i = 0; i < 50; ++i) {
        int px = dist_x(rng);
        int py = dist_y(rng);

        Eigen::Vector3f pixel_pos = top_left
            + right * ((px + 0.5f) * pixel_width)
            - up * ((py + 0.5f) * pixel_height);

        rays.emplace_back(props.location, pixel_pos - props.location);
    }

    std::ofstream out("../../Output/rays.txt");
    if (!out.is_open()) {
        std::cerr << "Failed to open rays.txt for writing.\n";
        return;
    }

    for (const auto& ray : rays) {
        out << ray.origin.x() << " " << ray.origin.y() << " " << ray.origin.z() << " "
            << ray.direction.x() << " " << ray.direction.y() << " " << ray.direction.z() << "\n";
    }

    std::cout << "Wrote " << rays.size() << " rays to rays.txt\n";
    out.close();
}