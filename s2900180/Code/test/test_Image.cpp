#include <gtest/gtest.h>
#include "Image.h"

TEST(PPMImageFileTest, CanReadPPM) {
    PPMImageFile img("./Output/test.ppm");
    EXPECT_NO_THROW(img.read_image_from_file());
}

TEST(PPMImageFileTest, CanUpdatePixel) {
    PPMImageFile img("data/example.ppm");
    img.read_image_from_file();
    img.update_pixel(0, 0, 255, 0, 0);
    Pixel p = img.get_pixel(0, 0);
    EXPECT_NEAR(p.r, 1.0f, 1e-5);
    EXPECT_NEAR(p.g, 0.0f, 1e-5);
    EXPECT_NEAR(p.b, 0.0f, 1e-5);
}