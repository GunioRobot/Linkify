#ifndef __NPMESSAGING__
#define __NPMESSAGING__


/** Major version number (year). */
#define MESSAGING_MAJOR_VERSION 2009

/** Minor version number (month). */
#define MESSAGING_MINOR_VERSION 02

/** Micro version number (day). */
#define MESSAGING_MICRO_VERSION 28

/** Full version string. */
#define MESSAGING_VERSION \
    MESSAGING_VERSION_IMPLEMENTATION(MESSAGING_MAJOR_VERSION, MESSAGING_MINOR_VERSION, MESSAGING_MICRO_VERSION)

#define MESSAGING_VERSION_IMPLEMENTATION(major, minor, micro) \
    MESSAGING_VERSION_TO_STRING(major) "-" MESSAGING_VERSION_TO_STRING(minor) "-" MESSAGING_VERSION_TO_STRING(micro)

#define MESSAGING_VERSION_TO_STRING(version) #version


#endif
