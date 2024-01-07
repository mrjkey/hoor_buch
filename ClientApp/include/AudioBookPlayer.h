#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <vector>
#include <iostream>

class AudioBookPlayer
{
public:
    void loadLibrary(const std::string &configFilePath);
    void selectBook(int index);

private:
    // std::vector<sf::Music> library;
    int currentBook;
};

void AudioBookPlayer::loadLibrary(const std::string &configFilePath)
{
    YAML::Node config = YAML::LoadFile(configFilePath);
    std::string directoryPath = config["audiobooks_directory"].as<std::string>();
    std::cout << "Loading audiobooks from " << directoryPath << std::endl;

    // for (const auto &entry : std::filesystem::directory_iterator(directoryPath))
    // {
    //     sf::Music book;
    //     if (book.openFromFile(entry.path().string()))
    //     {
    //         library.push_back(std::move(book));
    //     }
    // }
}

void AudioBookPlayer::selectBook(int index)
{
    // if (index >= 0 && index < library.size())
    // {
    //     currentBook = index;
    // }
}