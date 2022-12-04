#include <lore/logging.h>

#include <iostream>

namespace lore {

struct ConsoleLogger : public Logger {
    virtual std::ostream &log(Logger::Level level) override {
        switch (level) {
            case Logger::LOG_DEBUG:   return std::cout << "[debug] ";
            case Logger::LOG_INFO:    return std::cout << "[info ] ";
            case Logger::LOG_WARNING: return std::cerr << "[warn ] ";
            case Logger::LOG_ERROR:   return std::cerr << "[error] ";
        }
        return std::cout;
    }
};

std::shared_ptr<Logger> Logger::shared = std::make_shared<ConsoleLogger>();

}
