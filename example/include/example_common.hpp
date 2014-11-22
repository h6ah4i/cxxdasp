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

#ifndef EXAMPLE_COMMON_H_
#define EXAMPLE_COMMON_H_

#include <fstream>
#include <vector>
#include <string>

template <typename T>
static void read_raw_file(const char *filename, std::vector<T> &dest)
{
    std::ifstream ifs(filename, std::ios::binary);
    int length;

    ifs.seekg(0, ifs.end);
    length = static_cast<int>(ifs.tellg());
    ifs.seekg(0, ifs.beg);

    dest.resize(length / sizeof(T));

    ifs.read(reinterpret_cast<char *>(&dest[0]), dest.size() * sizeof(T));
}

template <typename T>
static void write_raw_file(const char *filename, const std::vector<T> &src)
{
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<const char *>(&src[0]), src.size() * sizeof(T));
}

#ifdef USE_CXXDASP
#include <cxxporthelper/aligned_memory.hpp>
#include <cxxdasp/filter/digital_filter.hpp>

template <typename T>
static void write_raw_file(const char *filename, const cxxporthelper::aligned_memory<T> &src)
{
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<const char *>(&src[0]), src.size() * sizeof(T));
}

cxxdasp::filter::filter_type_t parse_filter_type(const char *name)
{
    using namespace cxxdasp::filter;

    const std::string sname(name);

    if (sname == "lowpass") {
        return types::LowPass;
    } else if (sname == "highpass") {
        return types::HighPass;
    } else if (sname == "bandpass") {
        return types::BandPass;
    } else if (sname == "notch") {
        return types::Notch;
    } else if (sname == "peak") {
        return types::Peak;
    } else if (sname == "bell") {
        return types::Bell;
    } else if (sname == "lowshelf") {
        return types::LowShelf;
    } else if (sname == "highshelf") {
        return types::HighShelf;
    } else if (sname == "allpass") {
        return types::AllPass;
    } else {
        return types::Unknown;
    }
}
#endif

#endif // EXAMPLE_COMMON_H_
