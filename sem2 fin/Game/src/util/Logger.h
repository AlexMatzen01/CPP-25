#pragma once

#include <string>

namespace aq {

class Logger {
  public:
    static void Info(const std::string& message);
    static void Error(const std::string& message);
};

}  // namespace aq
