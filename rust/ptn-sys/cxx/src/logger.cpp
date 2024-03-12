#include <ptn/logger.h>

extern "C" {
    void ptn$logger$init();
    void ptn$logger$debug(const char* msg, const char* function, const char* filename, int line);
    void ptn$logger$info(const char* msg);
    void ptn$logger$warn(const char* msg);
    void ptn$logger$error(const char* msg);
}

void ptn::logger::init() {
    ptn$logger$init();
}

void ptn::logger::debug(const char* msg, const char* function = nullptr, const char* filename = nullptr, int line = -1) {
    ptn$logger$debug(msg, function, filename, line);
}

void ptn::logger::info(const char* msg) {
    ptn$logger$info(msg);
}

void ptn::logger::warn(const char* msg) {
    ptn$logger$warn(msg);
}

void ptn::logger::error(const char* msg) {
    ptn$logger$error(msg);
}