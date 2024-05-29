#include <ptn/logger.h>

extern "C" {
    void ptn$logger$init();
    void ptn$logger$debug(const char* msg, const char* function, const char* filename, int line);
    void ptn$logger$info(const char* msg, const char* function, const char* filename, int line);
    void ptn$logger$warn(const char* msg, const char* function, const char* filename, int line);
    void ptn$logger$error(const char* msg, const char* function, const char* filename, int line);
}

void ptn::logger::init() {
    ptn$logger$init();
}

void ptn::logger::debug(const char* msg, const char* function = nullptr, const char* filename = nullptr, int line = -1) {
    ptn$logger$debug(msg, function, filename, line);
}

void ptn::logger::info(const char* msg, const char* function = nullptr, const char* filename = nullptr, int line = -1) {
    ptn$logger$info(msg, function, filename, line);
}

void ptn::logger::warn(const char* msg, const char* function = nullptr, const char* filename = nullptr, int line = -1) {
    ptn$logger$warn(msg, function, filename, line);
}

void ptn::logger::error(const char* msg, const char* function = nullptr, const char* filename = nullptr, int line = -1) {
    ptn$logger$error(msg, function, filename, line);
}