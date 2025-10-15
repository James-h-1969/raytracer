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
    _image_map.at(py).at(px).r = r;
    _image_map.at(py).at(px).g = g;
    _image_map.at(py).at(px).b = b;
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

    // Normalized camera basis vectors from Blender export
    Eigen::Vector3f w_vec = props.gaze_vector_direction.normalized();  // Forward (-Y in Blender)
    Eigen::Vector3f up_vec = props.up_vector.normalized();             // Up (+Z in Blender)
    Eigen::Vector3f u_vec = up_vec.cross(w_vec).normalized();          // Right (+X)
    Eigen::Vector3f v_vec = w_vec.cross(u_vec).normalized();           // True up, orthogonalized

    float d = props.focal_length;

    for (int px = 0; px <= props.resolution_x; px += props.resolution_x / 5) {
        for (int py = 0; py <= props.resolution_y; py += props.resolution_y / 5) {

            // Offset pixel centers so image plane is exactly centered
            float u = ((static_cast<float>(px) / (props.resolution_x - 1)) - 0.5f) * props.sensor_width;
            float v = -props.sensor_height / 2 + (static_cast<float>(py)/ (props.resolution_y)) * props.sensor_height;

            std::cout << "v:" << v << std::endl;

            // Construct ray
            Ray current_ray;
            current_ray.origin = props.location;
            Eigen::Vector3f dir = (w_vec * d) + (u_vec * u) + (v_vec * v);  // Blender forward = -Y
            current_ray.direction = dir.normalized();

            rays.push_back(current_ray);
        }
    }

    // Write to file
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