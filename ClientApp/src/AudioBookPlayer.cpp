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
    if (currentBookIndex == -1)
    {
        std::cout << "No book selected" << std::endl;
    }
    else
    {
        std::cout << "Backing up book before switching" << std::endl;
        // update book in library
        update_audiobook(&library, currentBook);
        CreateOrUpdateAudiobookInfo(currentBook.path, currentBook);
    }
    std::cout << "Selecting book " << index << std::endl;

    // check that the index is valid
    if (index >= 0 && index < library.size())
    {
        is_playing = false;
        currentBookIndex = index;
        currentBook = library[index];
        // print the book info for debugging
        std::cout << "Book title: " << currentBook.title << std::endl;
        std::cout << "Book path: " << currentBook.path << std::endl;
        std::cout << "Book last played file: " << currentBook.last_played_file << std::endl;
        std::cout << "Book last played position: " << currentBook.last_played_position << std::endl;

        // check that the last played file exists, if not set it to the first file
        std::filesystem::path lastPlayedFilePath = std::filesystem::path(currentBook.path) / currentBook.last_played_file;
        // check if the last played file exists and is a file
        bool isFile = std::filesystem::is_regular_file(lastPlayedFilePath);
        if (!isFile)
        {
            std::cout << "Last played file doesn't exist. Setting it to the first file." << std::endl;
            currentBook.last_played_file = currentBook.files[0];
            // update the audiobook_info.yaml file
            CreateOrUpdateAudiobookInfo(currentBook.path, currentBook);
        }

        // Load a music to play
        std::filesystem::path musicFilePath = std::filesystem::path(currentBook.path) / currentBook.last_played_file;
        if (!music.openFromFile(musicFilePath.string()))
        {
            std::cerr << "Failed to load music" << std::endl;
            throw std::runtime_error("Failed to load music");
        }

        // update book in library
        update_audiobook(&library, currentBook);
    }
}

void AudioBookPlayer::play()
{
    // play the audio from the last played position
    music.play();
    is_playing = true;
    music.setPlayingOffset(sf::seconds(currentBook.last_played_position));
    // print the current playing offset for debugging
    std::cout << "Playing offset: " << music.getPlayingOffset().asSeconds() << std::endl;
    update_audiobook(&library, currentBook);
}

void AudioBookPlayer::pause()
{
    // pause the audio
    music.pause();
    is_playing = false;
    // store the current position in the last_played_position variable
    currentBook.last_played_position = music.getPlayingOffset().asSeconds();

    // print the current playing offset for debugging
    std::cout << "Paused offset: " << music.getPlayingOffset().asSeconds() << std::endl;

    // update the audiobook_info.yaml file
    CreateOrUpdateAudiobookInfo(currentBook.path, currentBook);
    update_audiobook(&library, currentBook);
}
