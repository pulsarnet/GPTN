#include <ptn/logger.h>

extern "C" {
    void ptn$logger$init();
    void ptn$logger$debug(const char* msg);
    void ptn$logger$info(const char* msg);
    void ptn$logger$warn(const char* msg);
    void ptn$logger$error(const char* msg);
}

void ptn::logger::init() {
    ptn$logger$init();
}

void ptn::logger::debug(const char* msg) {
    ptn$logger$debug(msg);
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