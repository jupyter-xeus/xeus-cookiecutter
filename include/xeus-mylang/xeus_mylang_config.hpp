/***************************************************************************
* Copyright (c) 2021,                                          
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/
#ifndef XEUS_MYLANG_CONFIG_HPP
#define XEUS_MYLANG_CONFIG_HPP

// Project version
#define XEUS_MYLANG_VERSION_MAJOR 0
#define XEUS_MYLANG_VERSION_MINOR 1
#define XEUS_MYLANG_VERSION_PATCH 0

// Composing the version string from major, minor and patch
#define XEUS_MYLANG_CONCATENATE(A, B) XEUS_MYLANG_CONCATENATE_IMPL(A, B)
#define XEUS_MYLANG_CONCATENATE_IMPL(A, B) A##B
#define XEUS_MYLANG_STRINGIFY(a) XEUS_MYLANG_STRINGIFY_IMPL(a)
#define XEUS_MYLANG_STRINGIFY_IMPL(a) #a

#define XEUS_MYLANG_VERSION XEUS_MYLANG_STRINGIFY(XEUS_MYLANG_CONCATENATE(XEUS_MYLANG_VERSION_MAJOR,   \
                 XEUS_MYLANG_CONCATENATE(.,XEUS_MYLANG_CONCATENATE(XEUS_MYLANG_VERSION_MINOR,   \
                                  XEUS_MYLANG_CONCATENATE(.,XEUS_MYLANG_VERSION_PATCH)))))

#ifdef _WIN32
    #ifdef XEUS_MYLANG_EXPORTS
        #define XEUS_MYLANG_API __declspec(dllexport)
    #else
        #define XEUS_MYLANG_API __declspec(dllimport)
    #endif
#else
    #define XEUS_MYLANG_API
#endif

#endif