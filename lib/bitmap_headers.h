#ifndef LABWORK_3_ACCELOLABS_BITMAP_HEADERS_H
#define LABWORK_3_ACCELOLABS_BITMAP_HEADERS_H

#include <cstdint>

struct BitmapHeaders {
    char data[54] = {};
    explicit BitmapHeaders (const uint32_t& width, const uint32_t& height, const uint32_t& padding) {
        uint32_t data_size = width * height * 3 + padding * height;
        uint32_t file_size = 54 + data_size;

        // [ Bitmap File Header ]

        // BM bytes
        data[0] = 'B';
        data[1] = 'M';

        // File size
        data[2] = char(file_size);
        data[3] = char(file_size >> 8);
        data[4] = char(file_size >> 16);
        data[5] = char(file_size >> 24);

        // Image data offset
        data[10] = 54;

        // [ Bitmap Info Header ]

        // Info header size
        data[14] = 40;

        // Width
        data[18] = char(width);
        data[19] = char(width >> 8);
        data[20] = char(width >> 16);
        data[21] = char(width >> 24);

        // Height
        data[22] = char(height);
        data[23] = char(height >> 8);
        data[24] = char(height >> 16);
        data[25] = char(height >> 24);

        // Color planes
        data[26] = 1;

        // Color depth
        data[28] = 24;

        data[34] = char(data_size);
        data[35] = char(data_size >> 8);
        data[36] = char(data_size >> 16);
        data[37] = char(data_size >> 24);
    }
};

#endif //LABWORK_3_ACCELOLABS_BITMAP_HEADERS_H
