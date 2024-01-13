// src/AudioBookPlayer.cpp
#include "AudioBookPlayer.h"
#include "LibraryManager.h"

void AudioBookPlayer::loadLibrary(const std::string &configFilePath)
{
    YAML::Node config = YAML::LoadFile(configFilePath);
    library_directory = config["audiobooks_directory"].as<std::string>();
    std::cout << "Loading audiobooks from " << library_directory << std::endl;

    // concatenate the library path with the library file name
    library_file_path = library_directory + "\\" + LIBRARY_FILE_NAME;
    std::cout << "Library path: " << library_file_path << std::endl;

    // if the library file doesn't exist, create it
    if (!std::filesystem::exists(library_file_path))
    {
        std::cout << "Library file doesn't exist. Creating it." << std::endl;
        CreateOrUpdateLibraryIndex(library_file_path, {});
    }

    // read the library file
    library = ReadLibraryIndex(library_file_path);
}

void AudioBookPlayer::selectBook(int index)
{
    std::cout << "Selecting book " << index << std::endl;

    // check that the index is valid
    if (index >= 0 && index < library.size())
    {
        currentBookIndex = index;
        currentBook = library[index];

        // check that the last played file exists, if not set it to the first file
        if (!std::filesystem::exists(currentBook.last_played_file))
        {
            std::cout << "Last played file doesn't exist. Setting it to the first file." << std::endl;
            currentBook.last_played_file = currentBook.files[0];
        }

        // Load a music to play
        if (!music.openFromFile(currentBook.path + "\\" + currentBook.last_played_file))
        {
            std::cerr << "Failed to load music" << std::endl;
            throw std::runtime_error("Failed to load music");
        }
    }
}

// void AudioBookPlayer::addBookD