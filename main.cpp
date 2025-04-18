#include <filesystem>
#include <iostream>

#include "lib/avalanche.h"
#include "lib/save_image.h"

struct Arguments {
    uint16_t width = 0;
    uint16_t length = 0;
    std::string output_path;
    std::string input_file;
    uint32_t max_iter = 0;
    uint32_t save_freq = 0;
    bool default_save_freq = true;
    bool default_input = true;
    bool default_output_path = true;
} arguments;

bool check_argument (const std::string& argument, const std::string& one_shot, const std::string& big_shot) {
    return argument == one_shot or argument == big_shot;
}

int main(int argc, char** argv) {

    // [ Input ]

    uint16_t curr_arg = 1;
    bool no_value_err = false;
    while (curr_arg < argc) {
        if (check_argument(argv[curr_arg], "-h", "--help")) {
            std::cout
                << "Abelian sandpile model by accelotron (aka accelolabs)."
                << "\n-h, --help \n\tthis menu."
                << "\n-l, --length \n\tlength of grid, required (16 bit)."
                << "\n-w, --width \n\twidth of grid, required (16 bit)."
                << "\n-i, --input \n\tinput tsv file, "
                << "\n\tif not provided, a dot of size ((length + width) * 12) will be placed in centre."
                << "\n-o, --output \n\toutput directory, "
                << "\n\tif not provided, output files will be saved in current directory."
                << "\n-m, --max-iter \n\tmaximum amount of iterations (32 bit), "
                << "\n\tif not provided, model will run until it is stable."
                << "\n-f, --freq \n\tfrequency of saving model states (32 bit), "
                << "\n\tif not provided, model will save only last state, "
                << "\n\tif 0 or 1, model will save every state." << std::endl;
            return 0;
        } else if (check_argument(argv[curr_arg], "-l", "--length")) {
            if (++curr_arg < argc) {
                arguments.length = std::stoul(argv[curr_arg]);
            } else {
                no_value_err = true;
            }
        } else if (check_argument(argv[curr_arg], "-w", "--width")) {
            if (++curr_arg < argc) {
                arguments.width = std::stoul(argv[curr_arg]);
            } else {
                no_value_err = true;
            }
        } else if (check_argument(argv[curr_arg], "-i", "--input")) {
            if (++curr_arg < argc) {
                arguments.input_file = std::string(argv[curr_arg]);
                arguments.default_input = false;
            } else {
                no_value_err = true;
            }
        } else if (check_argument(argv[curr_arg], "-o", "--output")) {
            if (++curr_arg < argc) {
                arguments.output_path = std::string(argv[curr_arg]);
                arguments.default_output_path = false;
            } else {
                no_value_err = true;
            }
        } else if (check_argument(argv[curr_arg], "-m", "--max-iter")) {
            if (++curr_arg < argc) {
                arguments.max_iter = std::stoul(argv[curr_arg]);
            } else {
                no_value_err = true;
            }
        } else if (check_argument(argv[curr_arg], "-f", "--freq")) {
            if (++curr_arg < argc) {
                arguments.save_freq = std::stoul(argv[curr_arg]);
                arguments.default_save_freq = false;
            } else {
                no_value_err = true;
            }
        } else {
            std::cerr
                << "Unknown parameter. "
                << "Use [" << argv[0] << " --help] "
                << "to display available options."
                << std::endl;

            return 1;
        }

        if (no_value_err) {
            std::cerr << "No value for " << argv[curr_arg - 1] << " parameter. " << std::endl;
            return 1;
        }

        curr_arg++;
    }

    // [ Argument check ]

    // Check if length and width are correct
    if (arguments.length == 0 or arguments.width == 0) {
        std::cerr
            << "Can't generate a grid of size "
            << arguments.length << "x" << arguments.width << std::endl;
        return 2;
    }

    // Check if save_freq is correct
    if (arguments.save_freq == 0) arguments.save_freq = 1;

    // Check if input file is empty
    if (!arguments.default_input and std::filesystem::is_empty(arguments.input_file)) {
        std::cerr
            << "This input file is empty -> "
            << arguments.input_file << std::endl;
        return 2;
    }

    // Check if output directory exists
    if (!arguments.default_output_path and !std::filesystem::is_directory(arguments.output_path)) {
        std::cerr
            << "This directory doesn't exist -> "
            << arguments.output_path << std::endl;
        return 2;
    }

    // [ Sandpile Variables ]

    const uint16_t the_width = arguments.width;
    const uint16_t the_length = arguments.length;
    const uint64_t field_size = the_width * the_length;

    auto* sand_pile_array = new uint64_t[field_size];
    for (uint64_t i = 0; i < field_size; ++i) sand_pile_array[i] = 0;

    auto* sand_pile_buffer = new uint64_t[field_size];

    // [ Init state ]

    if (arguments.default_input) {
        sand_pile_array[(the_length / 2) * the_width + the_width / 2] = (the_width + the_length) * 12;
    } else {
        std::ifstream input_file(arguments.input_file, std::ios::in);
        std::string input_data;
        uint16_t x;
        uint16_t y;
        bool added_x = false;
        char c;

        while (input_file.good() and !input_file.eof()) {
            c = char(input_file.get());

            if (c == '\t') {
                if (!added_x) {
                    x = std::stoull(input_data);
                    added_x = true;
                } else {
                    y = std::stoull(input_data);
                }
                input_data = "";
            } else if (c == '\n') {
                uint64_t index = (the_length * (the_width - y)) - (the_width - x);

                if (index < field_size and the_width >= x and the_length >= y) {
                    sand_pile_array[index] = std::stoull(input_data);
                } else {
                    std::cerr
                            << "Can't place a point "
                            << "(" << x << " " << y << ")"
                            << " to a grid "
                            << arguments.length << "x" << arguments.width << std::endl;
                    return 3;
                }

                added_x = false;
                input_data = "";
            } else {
                input_data += c;
            }
        }
    }

    // [ Main loop ]

    if (!arguments.default_save_freq) {
        std::filesystem::path output_file_init = arguments.output_path;
        output_file_init /= std::string("avalanche_0.bmp");

        SandArrayToBMP(output_file_init, sand_pile_array, the_width, the_length);
    }

    bool changed = true;
    uint64_t iteration_i = 0;

    while (changed) {
        iteration_i++;
        changed = Avalanche(sand_pile_array, sand_pile_buffer, the_width, the_length);

        if (!arguments.default_save_freq and iteration_i % arguments.save_freq == 0) {
            std::filesystem::path output_file = arguments.output_path;
            output_file /= std::string("avalanche_") + std::to_string(iteration_i) + std::string(".bmp");
            SandArrayToBMP(output_file, sand_pile_array, the_width, the_length);
        }

        if (arguments.max_iter > 0 and iteration_i >= arguments.max_iter) {
            break;
        }
    }

    std::filesystem::path output_file_last = arguments.output_path;
    output_file_last /= std::string("avalanche_") + std::to_string(iteration_i) + std::string(".bmp");

    SandArrayToBMP(output_file_last, sand_pile_array, the_width, the_length);

    return 0;
}