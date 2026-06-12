#ifndef TESTS_DEBUG_SETTINGS_H
#define TESTS_DEBUG_SETTINGS_H

#include <stdio.h>

#define DEBUG_SETTINGS_ENABLE_PRINTF

#ifdef DEBUG_SETTINGS_ENABLE_PRINTF

#define DEBUG_PRT(fmt, ...) printf(fmt, ##__VA_ARGS__); fflush(stdout)

#define DEBUG_PRTLN(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#else 

#define DEBUG(fmt, ...)
#define DEBUG_PRTLN(fmt, ...)

#endif

#endif  // TESTS_DEBUG_SETTINGS_H
