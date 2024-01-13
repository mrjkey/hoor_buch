// src/LibraryManager.cpp

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>
#include <filesystem>

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

std::vector<Audiobook> ReadLibraryIndex(const std::string &library_directory, const std::string &library_file_path)
{
    std::vector<Audiobook> library;
    YAML::Node config = YAML::LoadFile(library_file_path);
    for (const auto &book : config["audiobooks"])
    {
        std::string audiobook_directory = book["path"].as<std::string>();
        // combine path to library path + path
        audiobook_directory = library_directory + "\\" + audiobook_directory;
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

        // create the audiobook object from the audiobook_info.yaml file
        Audiobook audiobook = ReadAudiobookInfo(audiobook_directory);

        // update the audiobook_info.yaml file if the files in the directory don't match the files in the yaml file
        if (!files_match(audiobook_directory, audiobook))
        {
            CreateOrUpdateAudiobookInfo(audiobook_directory, audiobook);
        }

        library.push_back(audiobook);
    }
    return library;
}

bool files_match(const std::string &audiobookPath, Audiobook &audiobook)
{
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
        return false;
    }
    else
    {
        return true;
    }
}

void CreateOrUpdateAudiobookInfo(const std::string &audiobookPath, const Audiobook &audiobook)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "title" << YAML::Value << audiobook.title;
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
    audiobook.title = config["title"].as<std::string>();
    audiobook.progress = config["progress"].as<float>();
    audiobook.last_played_file = config["last_played_file"].as<std::string>();
    audiobook.last_played_position = config["last_played_position"].as<int>();
    for (const auto &file : config["files"])
    {
        audiobook.files.push_back(file.as<std::string>());
    }
    return audiobook;
}