/*
Image.h
James Hocking, 2025
*/

#pragma once 

#include "Types.h"
#include "Camera.h"
#include <string>
#include <fstream>
#include <random>
#include <vector>

struct Pixel {
    /*
        This struct holds the value at each value of the 
        image. For now, it holds both the index within the 
        image grid, as well as the colour in rgb.
    */

    float px;
    float py;

    Colour colour{0, 0, 0};

    Ray as_ray(CameraProperties props) {
        /*
        Function that converts the pixel into a ray based on the 
        Cameras properties, notably its positions and gaze.
        */

        // find the camera basis vectors
        Eigen::Vector3f w_vec = props.gaze_vector_direction.normalized(); 
        Eigen::Vector3f up_vec = props.up_vector.normalized();             
        Eigen::Vector3f u_vec = up_vec.cross(w_vec).normalized();          
        Eigen::Vector3f v_vec = w_vec.cross(u_vec).normalized();           

        // depending on the sensor fit, find the FOV
        float width, height;
        if (props.sensor_fit == SensorFit::HORIZONTAL) {
            width = props.sensor_width;
            height = width * (props.resolution_y/props.resolution_x);
        } else {
            height = props.sensor_height;
            width = height * (props.resolution_x/props.resolution_y);
        }

        // offset based on the pixel coordinates then multiply by basis vectors
        float d = props.focal_length;
        float u = ((static_cast<float>(px) / (props.resolution_x - 1)) - 0.5f) * width;
        float v = -height / 2 + (static_cast<float>(py)/ (props.resolution_y)) * height;
        Eigen::Vector3f dir = (w_vec * d) + (-u_vec * u) + (-v_vec * v);

        // construct the ray
        Ray ray;
        ray.origin = props.location;
        ray.direction = dir.normalized();

        return ray;
    };
};


class PPMImageFile {
    public:
        PPMImageFile(std::string filename): _filename(filename), _width(0), _height(0), _has_image(false) {};

        Pixel get_pixel(int px, int py) {return _image_map.at(py).at(px); };

        // function that reads ppm image from the specified file given in constructor
        void read_image_from_file();

        // function that writes the current updated (or not) image to a new file
        void write_current_image_to_file(std::string export_filename);

        // function to update the rgb values of a particular pixel based on its pixel coordinate
        void update_pixel(int px, int py, struct Colour colour);

        // setter 
        void set_width_and_height(int width, int height) {
            _width=width;
            _height=height;
            _image_map.resize(_height);
            for (auto& row : _image_map) {
                row.resize(_width);
            }
            for (int px = 0; px < get_width(); px++) {
                for (int py = 0; py < get_height(); py++) {
                    _image_map.at(py).at(px).px = px;
                    _image_map.at(py).at(px).py = py;
                }
            }
            
        };

        // getters 
        int get_width() {return _width;};
        int get_height() {return _height;};
        bool get_has_image() {return _has_image;};

    private:
        int _width;
        int _height;
        bool _has_image;
        std::string _filename;
        std::vector<std::vector<Pixel>> _image_map;
};