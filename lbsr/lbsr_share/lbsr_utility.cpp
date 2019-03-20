/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/stat.h> // stat
#include <cstdlib>    // std::env
#include <cctype>     // isdigit
#include <fstream>
#include <sstream>
#include <algorithm> // std::all_of
#include <stdsc/stdsc_exception.hpp>
#include <lbsr_share/lbsr_utility.hpp>

namespace lbsr_share
{

namespace utility
{

bool file_exist(const std::string& filename)
{
    std::ifstream ifs(filename, std::ios::binary);
    return ifs.is_open();
}

bool dir_exist(const std::string& dirname)
{
    struct stat stat_dir;
    return stat(dirname.c_str(), &stat_dir) == 0;
}

size_t file_size(const std::string& filename)
{
    size_t size = -1;
    if (!file_exist(filename))
    {
        std::ostringstream ss;
        ss << "failed to open " << filename;
        STDSC_THROW_FILE(ss.str());
    }
    else
    {
        std::ifstream ifs(filename, std::ios::binary);
        ifs.seekg(0, std::fstream::end);
        size_t epos = ifs.tellg();

        ifs.clear();

        ifs.seekg(0, std::fstream::beg);
        size_t bpos = ifs.tellg();

        size = epos - bpos;
    }
    return size;
}

std::string basename(const std::string& filepath)
{
    std::string filename = filepath;
    filename.erase(filename.begin(),
                   filename.begin() + filename.find_last_of("/") + 1);
    return filename;
}

bool isdigit(const std::string& str)
{
    return !str.empty() && std::all_of(str.cbegin(), str.cend(), ::isdigit);
}

std::string getenv(const char* env_var)
{
    std::string env;
    if (const char* env_p = std::getenv(env_var))
    {
        env = env_p;
    }
    return env;
}

void split(const std::string& str, const std::string& delims,
           std::vector<std::string>& vec_str)
{
    std::string::size_type index = str.find_first_not_of(delims);
    std::string::size_type str_size = str.size();
    vec_str.clear();
    while ((std::string::npos != index) && (0 <= str_size && index < str_size))
    {
        std::string::size_type next_index = str.find_first_of(delims, index);
        if ((std::string::npos == next_index) || (next_index > str_size))
        {
            next_index = str_size;
        }
        vec_str.push_back(str.substr(index, next_index - index));
        index = str.find_first_not_of(delims, next_index + 1);
    }
}

} /* namespace utility */

} /* namespace lbsr_share */
