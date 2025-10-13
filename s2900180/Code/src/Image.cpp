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