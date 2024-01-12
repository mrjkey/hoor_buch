// include/LibraryManager.h

// Purpose: Header file for LibraryManager.cpp.

#pragma once

#include "datatypes.h"

// Forward declarations
void CreateOrUpdateLibraryIndex(const std::string &libraryPath, const std::vector<Audiobook> &audiobooks);
std::vector<Audiobook> ReadLibraryIndex(const std::string &libraryPath);
void CreateOrUpdateAudiobookInfo(const std::string &audiobookPath, const Audiobook &audiobook);
Audiobook ReadAudiobookInfo(const std::string &audiobookPath);