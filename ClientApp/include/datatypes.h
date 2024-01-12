// include/datatypes.h

#pragma once

#include <string>
#include <vector>

struct Audiobook
{
    std::string title;
    std::string path;
    float progress;
    std::vector<std::string> files;
    std::string last_played_file;
    int last_played_position;
};
