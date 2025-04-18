#include "avalanche.h"

bool Avalanche (uint64_t*& sand_array, uint64_t*& sand_array_buffer, const uint16_t& width, const uint16_t& height) {
    bool changed = false;
    uint32_t field_size = width * height;

    for (uint64_t i = 0; i < field_size; ++i) sand_array_buffer[i] = 0;

    uint32_t curr_vert = 0;
    for (uint16_t i = 0; i < height; ++i) {
        for (uint16_t j = 0; j < width; ++j) {
            sand_array_buffer[curr_vert] += sand_array[curr_vert];

            if (sand_array[curr_vert] >= 4) {
                changed = true;
                sand_array_buffer[curr_vert] -= 4;

                if (j > 0) sand_array_buffer[curr_vert - 1] += 1; // Left
                if (j < width - 1) sand_array_buffer[curr_vert + 1] += 1; // Right
                if (i > 0) sand_array_buffer[curr_vert - width] += 1; // Top
                if (i < height - 1) sand_array_buffer[curr_vert + width] += 1; // Bottom
            }
            curr_vert++;
        }
    }

    // for (uint32_t i = 0; i < field_size; ++i) sand_array[i] = sand_array_temp[i];
    std::swap(sand_array, sand_array_buffer);
    // delete[] sand_array_temp;

    return changed;
}


