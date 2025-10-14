/*
Image.h
James Hocking, 2025
*/

#pragma once 

#include "Light.h"
#include "Camera.h"
#include <string>
#include <fstream>
#include <random>
#include <vector>

void generate_ray_text_file(Camera& camera);

struct Pixel {
    /*
        This struct holds the value at each value of the 
        image. For now, it holds both the index within the 
        image grid, as well as the colour in rgb.
    */

    int px;
    int py;

    float r;
    float g;
    float b;

    Ray& as_ray(Camera& camera) {
        Ray ray;
        ray.origin = camera.get_camera_properties().location;


        /*
        
        My understanding so far is to take current location + d * central direction vector 
        and then dependening on row and column, and width and height of the image then 
        we split it up and find the value.
        
        */

        return ray;
    };
};

class PPMImageFile {
    public:
        PPMImageFile(std::string filename): _filename(filename) {};
        Pixel get_pixel(int x, int y) {return _image_map.at(y).at(x); };
        void read_image_from_file();
        void write_current_image_to_file(std::string export_filename);
        void update_pixel(int px, int py, int r, int g, int b);
    private:
        std::string _filename;
        int _width;
        int _height;
        std::vector<std::vector<Pixel>> _image_map;
};