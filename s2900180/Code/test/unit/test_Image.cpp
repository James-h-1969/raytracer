#include <gtest/gtest.h>
#include <cstdio>
#include "Image.h"

TEST(PPMImageFileTest, CanReadPPM) {
    std::string filepath = std::string(TEST_DATA_DIR) + "/test.ppm";
    PPMImageFile img = PPMImageFile(filepath);
    EXPECT_NO_THROW(img.read_image_from_file());
    ASSERT_EQ(img.get_height(), 4);
    ASSERT_EQ(img.get_width(), 4);
    ASSERT_EQ(img.get_pixel(3, 0).colour.r, 15);
    ASSERT_EQ(img.get_pixel(3, 0).colour.g, 0);
    ASSERT_EQ(img.get_pixel(3, 0).colour.b, 15);
}

TEST(PPMImageFileTest, CanUpdatePixel) {
    std::string filepath = std::string(TEST_DATA_DIR) + "/test.ppm";
    PPMImageFile img = PPMImageFile(filepath);
    EXPECT_NO_THROW(img.read_image_from_file());

    ASSERT_EQ(img.get_pixel(3, 0).colour.r, 15);
    ASSERT_EQ(img.get_pixel(3, 0).colour.g, 0);
    ASSERT_EQ(img.get_pixel(3, 0).colour.b, 15);

    Colour new_colour{3, 1, 15};
    img.update_pixel(3, 0, new_colour);

    ASSERT_EQ(img.get_pixel(3, 0).colour.r, 3);
    ASSERT_EQ(img.get_pixel(3, 0).colour.g, 1);
    ASSERT_EQ(img.get_pixel(3, 0).colour.b, 15);
}

TEST(PPMImageFileTest, CanWriteToFile) {
    std::string filepath = std::string(TEST_DATA_DIR) + "/test.ppm";
    PPMImageFile img = PPMImageFile(filepath);
    EXPECT_NO_THROW(img.read_image_from_file());

    std::string filepath_output = std::string(TEST_DATA_DIR) + "/test_output.ppm";
    img.write_current_image_to_file(filepath_output);

    PPMImageFile img2 = PPMImageFile(filepath_output);
    EXPECT_NO_THROW(img2.read_image_from_file());

    ASSERT_EQ(img2.get_pixel(3, 0).colour.r, 15);
    ASSERT_EQ(img2.get_pixel(3, 0).colour.g, 0);
    ASSERT_EQ(img2.get_pixel(3, 0).colour.b, 15);

    // remove the created file after use
    if (std::remove(filepath_output.c_str()) == 0) {
        std::cout << "File deleted successfully.\n";
    } else {
        std::perror("Error deleting file");
    }
}
