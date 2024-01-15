// src/LibraryManager.cpp

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <taglib/tag.h>
#include <taglib/fileref.h>

#include "LibraryManager.h"

// Function implementations
void CreateOrUpdateLibraryIndex(const std::string &libraryPath, const std::vector<Audiobook> &library)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "audiobooks" << YAML::Value << YAML::BeginSeq;
    for (const auto &book : library)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "title" << YAML::Value << book.title;
        out << YAML::Key << "path" << YAML::Value << book.path;
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(libraryPath);
    fout << out.c_str();
}

std::vector<Audiobook> ReadLibraryIndex(const std::string &library_file_path)
{
    std::vector<Audiobook> library;
    YAML::Node config = YAML::LoadFile(library_file_path);
    for (const auto &book : config["audiobooks"])
    {
        std::string audiobook_directory = book["path"].as<std::string>();
        // convert combined path to absolute path, and convert it to a string
        audiobook_directory = std::filesystem::absolute(audiobook_directory).string();

        // check that the directory, if not don't add the book and display a warning
        if (!std::filesystem::exists(audiobook_directory))
        {
            std::cout << "Directory " << audiobook_directory << " doesn't exist. Skipping." << std::endl;
            continue;
        }

        // check that the audiobook_info.yaml file exists, if it doesn't, create it
        if (!std::filesystem::exists(audiobook_directory + "/audiobook_info.yaml"))
        {
            std::cout << "audiobook_info.yaml doesn't exist. Creating it." << std::endl;
            CreateOrUpdateAudiobookInfo(audiobook_directory, {});
        }

        add_new_audiobook(&library, audiobook_directory);
    }
    return library;
}

// add a new audiobook to the library
void add_new_audiobook(std::vector<Audiobook> *library, const std::string audiobook_directory)
{
    // try to read the audiobook_info.yaml file
    Audiobook audiobook;
    try
    {
        // create the audiobook object from the audiobook_info.yaml file
        audiobook = ReadAudiobookInfo(audiobook_directory);
    }
    // if there is an error, create a new audiobook object
    catch (const std::exception &e)
    {
        std::cout << "Error reading audiobook_info.yaml file. Creating a new one." << e.what() << std::endl;
        CreateOrUpdateAudiobookInfo(audiobook_directory, {});
        audiobook = ReadAudiobookInfo(audiobook_directory);
    }

    // update the audiobook_info.yaml file if the files in the directory don't match the files in the yaml file
    bool changed = files_changed(audiobook_directory, audiobook);

    // if the title is empty, set it to the directory name
    if (audiobook.title.empty())
    {
        audiobook.title = std::filesystem::path(audiobook_directory).filename().string();
        changed = true;
    }

    float duration = get_book_duration(audiobook_directory);
    if (audiobook.duration != duration)
    {
        audiobook.duration = duration;
        changed = true;
    }

    // if any changes were made to the audiobook_info.yaml file, update it
    if (changed)
    {
        CreateOrUpdateAudiobookInfo(audiobook_directory, audiobook);
    }

    library->push_back(audiobook);
}

// remove duplicate audiobooks from the library
void remove_duplicate_audiobooks(std::vector<Audiobook> *library)
{
    for (auto it = library->begin(); it != library->end(); ++it)
    {
        auto it2 = it;
        ++it2; // Start from the next element after 'it'
        while (it2 != library->end())
        {
            if (it->path == it2->path)
            {
                std::cout << "Removing duplicate audiobook: " << it->path << std::endl;
                it2 = library->erase(it2); // Erase and update it2 to the next valid iterator
            }
            else
            {
                ++it2; // Only increment it2 if no erasure happened
            }
        }
    }
}

float get_book_duration(const std::string &audiobookPath)
{
    float duration = 0;
    // list of file extensions to look for
    std::vector<std::string> file_extensions = {".mp3", ".m4a", ".m4b", ".wav", ".ogg", ".flac"};

    // get the list of files in the directory
    for (const auto &entry : std::filesystem::directory_iterator(audiobookPath))
    {
        // check if the file is an audio file
        for (const auto &extension : file_extensions)
        {
            if (entry.path().extension() == extension)
            {
                try
                {
                    // read the file metadata
                    TagLib::FileRef f(entry.path().string().c_str());

                    // get the duration of the file from the metadata
                    int file_duration = f.audioProperties()->length();

                    // add the duration to the total duration
                    duration += file_duration;
                }
                // catch TagLib: FileRef::audioProperties() - Called without a valid file.
                catch (const std::exception &e)
                {
                    std::cout << "Error reading file metadata: " << e.what() << std::endl;
                }

                // break out of the loop
                break;
            }
        }
    }
    return duration;
}

bool files_changed(const std::string &audiobookPath, Audiobook &audiobook)
{
    bool changed = false;
    // check the files in the directory and add them to the yaml file
    // any audio files in the directory that aren't in the yaml file should be added
    // any audio files in the yaml file that aren't in the directory should be removed

    // list of file extensions to look for
    std::vector<std::string> file_extensions = {".mp3", ".m4a", ".m4b", ".wav", ".ogg", ".flac"};

    // get the list of files in the directory
    std::vector<std::string> files;
    for (const auto &entry : std::filesystem::directory_iterator(audiobookPath))
    {
        // check if the file is an audio file
        for (const auto &extension : file_extensions)
        {
            if (entry.path().extension() == extension)
            {
                files.push_back(entry.path().filename().string());
                break;
            }
        }
    }
    if (files != audiobook.files)
    {
        audiobook.files = files;
        changed = true;
    }

    // check if the last played file is in the list of files
    // if not set the last played file to empty and reset the last played position and progress
    if (std::find(files.begin(), files.end(), audiobook.last_played_file) == files.end())
    {
        // if the last played was already empty, don't change anything
        if (!audiobook.last_played_file.empty())
        {
            audiobook.last_played_file = "";
            audiobook.last_played_position = 0;
            audiobook.progress = 0;
            changed = true;
        }
    }

    return changed;
}

void CreateOrUpdateAudiobookInfo(const std::string &audiobookPath, const Audiobook &audiobook)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "title" << YAML::Value << audiobook.title;
    out << YAML::Key << "duration" << YAML::Value << audiobook.duration;
    out << YAML::Key << "progress" << YAML::Value << audiobook.progress;
    out << YAML::Key << "files" << YAML::Value << YAML::Flow << audiobook.files;
    out << YAML::Key << "last_played_file" << YAML::Value << audiobook.last_played_file;
    out << YAML::Key << "last_played_position" << YAML::Value << audiobook.last_played_position;
    out << YAML::EndMap;

    std::ofstream fout(audiobookPath + "/audiobook_info.yaml");
    fout << out.c_str();
}

Audiobook ReadAudiobookInfo(const std::string &audiobookPath)
{
    Audiobook audiobook;
    YAML::Node config = YAML::LoadFile(audiobookPath + "/audiobook_info.yaml");
    audiobook.path = audiobookPath;
    audiobook.title = config["title"].as<std::string>();
    // backwards compatibility
    // if duration is not in the yaml file, set it to 0
    if (!config["duration"])
    {
        audiobook.duration = 0;
    }
    else
    {
        audiobook.duration = config["duration"].as<float>();
    }
    audiobook.progress = config["progress"].as<float>();
    audiobook.last_played_file = config["last_played_file"].as<std::string>();
    audiobook.last_played_position = config["last_played_position"].as<int>();
    for (const auto &file : config["files"])
    {
        audiobook.files.push_back(file.as<std::string>());
    }
    return audiobook;
}