// src/LibraryManager.cpp

#include <yaml-cpp/yaml.h>
#include <fstream>

#include "LibraryManager.h"

// Function implementations
void CreateOrUpdateLibraryIndex(const std::string &libraryPath, const std::vector<Audiobook> &audiobooks)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "audiobooks" << YAML::Value << YAML::BeginSeq;
    for (const auto &book : audiobooks)
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

std::vector<Audiobook> ReadLibraryIndex(const std::string &libraryPath)
{
    std::vector<Audiobook> audiobooks;
    YAML::Node config = YAML::LoadFile(libraryPath);
    for (const auto &book : config["audiobooks"])
    {
        Audiobook audiobook;
        audiobook.title = book["title"].as<std::string>();
        audiobook.path = book["path"].as<std::string>();
        audiobooks.push_back(audiobook);
    }
    return audiobooks;
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