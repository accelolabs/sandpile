#ifndef LABWORK_3_ACCELOLABS_SAVE_IMAGE_H
#define LABWORK_3_ACCELOLABS_SAVE_IMAGE_H

#include <string>
#include <cstdint>
#include <fstream>

#include "bitmap_headers.h"
#include "colors.h"

void SandArrayToBMP (const std::string& file_name, const uint64_t* sand_array, const uint16_t& width, const uint16_t& height);

#endif //LABWORK_3_ACCELOLABS_SAVE_IMAGE_H
