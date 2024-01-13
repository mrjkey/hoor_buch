// include/datatypes.h

#pragma once

#include <string>
#include <vector>

#define CONFIG_FILE_NAME "config.yaml"
#define LIBRARY_FILE_NAME "library.yaml"
#define AUDIOBOOK_METADATA_FILE_NAME "audiobook_info.yaml"

struct Audiobook
{
    std::string path;
    std::string title;
    std::string author;
    float progress;
    float duration;
    std::vector<std::string> files;
    std::string last_played_file;
    int last_played_position;
};
