// include/AudioBookPlayer.h

// Purpose: Header file for AudioBookPlayer.cpp

#pragma once

#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <vector>
#include <iostream>
#include <SFML/Audio.hpp>

#include "datatypes.h"

class AudioBookPlayer
{
public:
    void loadLibrary(const std::string &configFilePath);
    void selectBook(int index);

    // private:
    std::vector<Audiobook> library;
    std::string library_directory;
    std::string library_file_path;
    sf::Music music;
    Audiobook currentBook;
};
