#include "Image.h"

void PPMImageFile::read_image_from_file() {
    // open the file 
    std::ifstream file(_filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file.\n";
        throw std::runtime_error(_filename);
    }

    std::string format;
    file >> format;
    if (format != "P3") {
        std::cerr << "Error: Unsupported PPM format (expected P3, got " << format << ")" << std::endl;
        return;
    }

    file >> _width >> _height;

    int max_colour;
    file >> max_colour;

    _image_map.clear();

    for (int py = 0; py < _height; py++) {
        std::vector<Pixel> row = {};
        for (int px = 0; px < _width; px++) {
            int r, g, b;
            file >> r >> g >> b;
            
            // create the pixel from the row
            Pixel p;
            p.px = px;
            p.py = py;
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

void generate_ray_text_file(Camera camera, std::string output_file_name, int amount_of_rays) {
    const auto& props = camera.get_camera_properties();
    std::vector<Ray> rays;

    int step_x = std::max(1, static_cast<int>(props.resolution_x / amount_of_rays));
    int step_y = std::max(1, static_cast<int>(props.resolution_y / amount_of_rays));

    for (int px = 0; px <= props.resolution_x; px += step_x) {
        for (int py = 0; py <= props.resolution_y; py += step_y) {
            Pixel p(px, py, 0, 0, 0);
            Ray r = p.as_ray(props);
            rays.push_back(r);
        }
    }

    // write to file
    std::ofstream out(output_file_name);
    if (!out.is_open()) {
        std::cerr << "Failed to open rays.txt for writing.\n";
        return;
    }

    for (const auto& ray : rays) {
        out << ray.origin.x() << " " << ray.origin.y() << " " << ray.origin.z() << " "
            << ray.direction.x() << " " << ray.direction.y() << " " << ray.direction.z() << "\n";
    }

    out.close();
}