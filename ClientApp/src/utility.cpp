#include <string>

// a function that will convert a float or integer in seconds to a string formated like: 1h 23m 45s
std::string seconds_to_string(float seconds)
{
    int hours = seconds / 3600;
    int minutes = (seconds - (hours * 3600)) / 60;
    int secs = seconds - (hours * 3600) - (minutes * 60);
    std::string time_string = std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    return time_string;
}