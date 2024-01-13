// include/LibraryManager.h

// Purpose: Header file for LibraryManager.cpp.

#pragma once

#include "datatypes.h"

// Forward declarations
void CreateOrUpdateLibraryIndex(const std::string &libraryPath, const std::vector<Audiobook> &audiobooks);
std::vector<Audiobook> ReadLibraryIndex(const std::string &library_file_path);
void CreateOrUpdateAudiobookInfo(const std::string &audiobookPath, const Audiobook &audiobook);
Audiobook ReadAudiobookInfo(const std::string &audiobookPath);
bool files_changed(const std::string &audiobookPath, Audiobook &audiobook);
float get_book_duration(const std::string &audiobookPath);
void add_new_audiobook(std::vector<Audiobook> *library, const std::string audiobook_directory);
void remove_duplicate_audiobooks(std::vector<Audiobook> *library);