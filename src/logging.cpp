#include <lore/logging.h>

#include <iostream>
#include <sstream>

namespace lore {

struct LoggingStringbuf : public std::stringbuf {
    Logger::Level level;
    LoggingStringbuf(Logger::Level level)
        : level(level) {}

    virtual int sync() {
        std::ostream &stream = level >= Logger::LOG_WARNING ? std::cerr : std::cout;
        stream << "[" << "DIWE"[level] << "] " << str() << std::endl;
        str("");
        return 0;
    }
};

struct ConsoleLogger : public Logger {
    struct Channel {
        std::unique_ptr<LoggingStringbuf> stringbuf;
        std::ostream stream;

        Channel(Level level)
            : stringbuf(std::make_unique<LoggingStringbuf>(level)),
              stream(stringbuf.get()) {

        }
    };

    Channel debug { LOG_DEBUG };
    Channel info { LOG_INFO };
    Channel warning { LOG_WARNING };
    Channel error { LOG_ERROR };

    virtual std::ostream &log(Logger::Level level) override {
        switch (level) {
            case Logger::LOG_DEBUG:   return debug.stream;
            case Logger::LOG_INFO:    return info.stream;
            case Logger::LOG_WARNING: return warning.stream;
            case Logger::LOG_ERROR:   return error.stream;
        }
        return error.stream;
    }
};

std::shared_ptr<Logger> Logger::shared = std::make_shared<ConsoleLogger>();

}
