#include "Utils.h"
#include <string>
#include <iomanip>
#include <sstream>

 std::string Utils::formatCents(int cents){
            std::ostringstream oss;
            oss<<(cents/100)<<'.'<<std::setw(2)<<std::setfill('0')<<(cents %100);
            return oss.str();
        }