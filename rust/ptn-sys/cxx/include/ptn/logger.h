#ifndef LOGGER_H
#define LOGGER_H

namespace ptn::logger {
    void init();
    void debug(const char* msg, const char* function, const char* filename, int line);
    void info(const char* msg);
    void warn(const char* msg);
    void error(const char* msg);
}

#endif //LOGGER_H
