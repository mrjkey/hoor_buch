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
    void play();
    void pause();
    void rewind(int x);
    void fast_forward(int x);

    // private:
    std::vector<Audiobook> library;
    std::string library_directory;
    std::string library_file_path;
    sf::Music music;
    Audiobook currentBook;
    int currentBookIndex = -1;
    bool is_playing = false;
};
