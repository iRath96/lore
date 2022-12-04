#pragma once

#include <ostream>

namespace lore {

struct Logger {
    enum Level {
        LOG_DEBUG = 0,
        LOG_INFO = 1,
        LOG_WARNING = 2,
        LOG_ERROR = 3,
    };

    virtual std::ostream &log(Level level) = 0;

    static std::shared_ptr<Logger> shared;
};

namespace log {

static std::ostream &debug() { return Logger::shared->log(Logger::LOG_DEBUG); }
static std::ostream &info() { return Logger::shared->log(Logger::LOG_INFO); }
static std::ostream &warning() { return Logger::shared->log(Logger::LOG_WARNING); }
static std::ostream &error() { return Logger::shared->log(Logger::LOG_ERROR); }

}

}
