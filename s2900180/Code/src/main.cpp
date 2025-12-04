#include "Raytracer.h"

int main(int argc, char* argv[]) {
  RayTracer raytracer;

  raytracer.create_settings_from_command_args(argc, argv);
  raytracer.setup();

  raytracer.render_image();
  return 0;
}
