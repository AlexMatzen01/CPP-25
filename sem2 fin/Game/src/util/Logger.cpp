#include "util/Logger.h"

#include <iostream>

namespace aq {

void Logger::Info(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void Logger::Error(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

}  // namespace aq
