// include/AudioBookPlayer.h

// Purpose: Header file for AudioBookPlayer.cpp

#pragma once

#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <vector>
#include <iostream>

#include "datatypes.h"

class AudioBookPlayer
{
public:
    void loadLibrary(const std::string &configFilePath);
    void selectBook(int index);

private:
    // std::vector<sf::Music> library;
    int currentBook;
};
