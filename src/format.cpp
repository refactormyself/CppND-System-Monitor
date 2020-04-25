#include <string>
#include <sstream>

#include "format.h"

using std::string;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 

long minutes = seconds/60;
long hour = minutes/60;

std::ostringstream time;
time << hour << ':' << minutes%60 << ':' << seconds%60;

return time.str();

}
