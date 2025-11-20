#include "Raytracer.h"

int main() {
  RayTracer raytracer;
  raytracer.setup("../../ASCII/test1.json");
  raytracer.render_image("../../Output/test1.ppm");
  return 0;
}
