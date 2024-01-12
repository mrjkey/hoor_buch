// include/BookProgress.h

#pragma once

#include <string>

class BookProgress
{
public:
    void saveProgress(std::string filename, int position);
    int loadProgress(std::string filename);
};
