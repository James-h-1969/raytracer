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
            Colour colour(r, g, b);
            p.colour = colour;
            row.push_back(p);
        }
        _image_map.push_back(row);
    }

    _has_image = true;
}

void PPMImageFile::update_pixel(int px, int py, struct Colour new_colour) {
    _image_map.at(py).at(px).colour = new_colour;
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
            out << p.colour.r << " " << p.colour.g << " " << p.colour.b << "  ";
        }
        out << "\n";
    }

    out.close();    
}