// include/BookProgress.h

#ifndef BOOKPROGRESS_H
#define BOOKPROGRESS_H

#include <string>

class BookProgress
{
public:
    void saveProgress(std::string filename, int position);
    int loadProgress(std::string filename);
};

#endif // BOOKPROGRESS_H