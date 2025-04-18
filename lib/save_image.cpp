#include "save_image.h"

void SandArrayToBMP (const std::string& file_name, const uint64_t* sand_array, const uint16_t& width, const uint16_t& length) {
    const uint16_t image_padding = 4 - (width * 3) % 4;
    BitmapHeaders bitmap_full_header (width, length, image_padding);
    ColorTable color_table;

    std::ofstream file(file_name, std::ios::binary);

    file.write((char*) &bitmap_full_header, 54);

    uint32_t curr_vert = length * width;
    for (uint16_t i = 0; i < length; ++i) {
        curr_vert -= width;

        for (uint16_t j = 0; j < width; ++j) {
            unsigned char* pixel = color_table.black;

            switch (sand_array[curr_vert]) {
                case 0:
                    pixel = color_table.white;
                    break;
                case 1:
                    pixel = color_table.green;
                    break;
                case 2:
                    pixel = color_table.purple;
                    break;
                case 3:
                    pixel = color_table.yellow;
                    break;
            }

            file.write((char*) pixel, 3);
            curr_vert++;
        }
        curr_vert -= width;

        if (image_padding % 4 != 0) {
            for (uint16_t j = 0; j < image_padding; ++j) {
                char zero = 0;
                file.write(&zero, 1);
            }
        }
    }

    file.close();
}
