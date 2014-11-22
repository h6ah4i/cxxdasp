//
//    Copyright (C) 2014 Haruki Hasegawa
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include <cxxdasp/cxxdasp.hpp>
#include <cxxdasp/window/window_functions.hpp>

#define USE_CXXDASP
#include "example_common.hpp"

using namespace cxxdasp;

static void print_usage(const char *exe_name)
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    " << exe_name << " window_type size" << std::endl;
    std::cout << "window_type = rect|hann|hamm|blackman|flattop" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char const *argv[])
{
    // initialize
    cxxdasp_init();

    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    // parse arguments
    std::string type = argv[1];
    const int size = atoi(argv[2]);

    if (!(size >= 1 && size <= 32678)) {
        std::cerr << "Invalid window size: " << size << std::endl;
        return 1;
    }

    // make window data
    std::vector<float> window;

    window.resize(size);

    if (type == "rect") {
        window::generate_rectangular_window(&window[0], size);
    } else if (type == "hann") {
        window::generate_hann_window(&window[0], size);
    } else if (type == "hamm") {
        window::generate_hamming_window(&window[0], size);
    } else if (type == "blackman") {
        window::generate_blackman_window(&window[0], size);
    } else if (type == "flattop") {
        window::generate_flat_top_window(&window[0], size);
    } else {
        std::cerr << "Invalid window type: " << type << std::endl;
        return 1;
    }

    // output
    for (int i = 0; i < size; ++i) {
        std::cout << i << "\t" << window[i] << std::endl;
    }

    return 0;
}
