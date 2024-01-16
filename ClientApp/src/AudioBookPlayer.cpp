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
        // update_audiobook(&library, *currentBook);
        CreateOrUpdateAudiobookInfo(currentBook->path, *currentBook);
    }
    std::cout << "Selecting book " << index << std::endl;

    // check that the index is valid
    if (index >= 0 && index < library.size())
    {
        is_playing = false;
        currentBookIndex = index;
        currentBook = &library[index];
        // print the book info for debugging
        std::cout << "Book title: " << currentBook->title << std::endl;
        std::cout << "Book path: " << currentBook->path << std::endl;
        std::cout << "Book last played file: " << currentBook->last_played_file << std::endl;
        std::cout << "Book last played position: " << currentBook->last_played_position << std::endl;

        // check that the last played file exists, if not set it to the first file
        std::filesystem::path lastPlayedFilePath = std::filesystem::path(currentBook->path) / currentBook->last_played_file;
        // check if the last played file exists and is a file
        bool isFile = std::filesystem::is_regular_file(lastPlayedFilePath);
        if (!isFile)
        {
            std::cout << "Last played file doesn't exist. Setting it to the first file." << std::endl;
            currentBook->last_played_file = currentBook->files[0];
            // update the audiobook_info.yaml file
            CreateOrUpdateAudiobookInfo(currentBook->path, *currentBook);
        }

        // Load a music to play
        std::filesystem::path musicFilePath = std::filesystem::path(currentBook->path) / currentBook->last_played_file;
        if (!music.openFromFile(musicFilePath.string()))
        {
            std::cerr << "Failed to load music" << std::endl;
            throw std::runtime_error("Failed to load music");
        }

        // update book in library
        update_audiobook(&library, *currentBook);
    }
}

void AudioBookPlayer::play()
{
    // play the audio from the last played position
    music.play();
    is_playing = true;
    music.setPlayingOffset(sf::seconds(currentBook->last_played_position));
    // print the current playing offset for debugging
    std::cout << "Playing offset: " << music.getPlayingOffset().asSeconds() << std::endl;
    update_audiobook(&library, *currentBook);
}

void AudioBookPlayer::pause()
{
    // pause the audio
    music.pause();
    is_playing = false;
    // store the current position in the last_played_position variable
    currentBook->last_played_position = music.getPlayingOffset().asSeconds();

    // print the current playing offset for debugging
    std::cout << "Paused offset: " << music.getPlayingOffset().asSeconds() << std::endl;

    // update the audiobook_info.yaml file
    CreateOrUpdateAudiobookInfo(currentBook->path, *currentBook);
    update_audiobook(&library, *currentBook);
}

// time control functions
void AudioBookPlayer::rewind(int x)
{
    if (is_playing)
    {
        if (music.getPlayingOffset().asSeconds() < x)
        {
            music.setPlayingOffset(sf::seconds(0));
        }
        else
        {
            music.setPlayingOffset(music.getPlayingOffset() - sf::seconds(x));
        }
        // store the current position in the last_played_position variable
        currentBook->last_played_position = music.getPlayingOffset().asSeconds();
    }
    else
    {
        currentBook->last_played_position -= x;
    }

    // print the current playing offset for debugging
    std::cout << "Rewind offset: " << music.getPlayingOffset().asSeconds() << std::endl;
}

void AudioBookPlayer::fast_forward(int x)
{
    if (is_playing)
    {
        if (music.getPlayingOffset().asSeconds() + x > music.getDuration().asSeconds())
        {
            music.setPlayingOffset(music.getDuration());
        }
        else
        {
            music.setPlayingOffset(music.getPlayingOffset() + sf::seconds(x));
        }
        // fast forward the audio by x seconds
        music.setPlayingOffset(music.getPlayingOffset() + sf::seconds(x));
        // store the current position in the last_played_position variable
        currentBook->last_played_position = music.getPlayingOffset().asSeconds();
    }
    else
    {
        currentBook->last_played_position += x;
    }

    // print the current playing offset for debugging
    std::cout << "Fast forward offset: " << music.getPlayingOffset().asSeconds() << std::endl;
}

float AudioBookPlayer::GetBookProgress(int index)
{
    // get the book by index
    Audiobook *book = &library[index];

    float progress = 0.0f;
    // add the duration of all the files before the current file
    for (int i = 0; i < book->files.size(); i++)
    {
        // if the current file is the last played file, break
        if (book->files[i] == book->last_played_file)
        {
            break;
        }
        else
        {
            // add the duration of the file
            progress += book->file_durations[book->files[i]];
        }
    }
    // add the last played position
    progress += book->last_played_position;

    // divide by the total duration
    book->progress = progress / book->duration;

    return progress; // return the progress in seconds
}