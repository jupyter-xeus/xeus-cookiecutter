############################################################################
# Copyright (c) 2016, Martin Renou, Johan Mabille, Sylvain Corlay, and     #
# Wolf Vollprecht                                                          #
# Copyright (c) 2016, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.4.3)
project(xeus-wren)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(XEUS_WREN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(STRINGS "${XEUS_WREN_INCLUDE_DIR}/xeus-wren/xeus_wren_config.hpp" xwren_version_defines
     REGEX "#define XWREN_VERSION_(MAJOR|MINOR|PATCH)")
foreach (ver ${xwren_version_defines})
    if (ver MATCHES "#define XWREN_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XWREN_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif ()
endforeach ()
set(${PROJECT_NAME}_VERSION
    ${XWREN_VERSION_MAJOR}.${XWREN_VERSION_MINOR}.${XWREN_VERSION_PATCH})
message(STATUS "Building xeus-wren v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

if (NOT DEFINED XWREN_KERNELSPEC_PATH)
    set(XWREN_KERNELSPEC_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/")
endif ()

if (NOT DEFINED XWREN_LUA_EXTRA_PACKAGE_PATH)
    set(XWREN_LUA_EXTRA_PACKAGE_PATH "${CMAKE_INSTALL_PREFIX}/share/jupyter/kernels/xwren/wren_packages")
endif ()

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xwren/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xwren/kernel.json"
)

# Build options
# =============

# Compilation options
OPTION(XWREN_DISABLE_ARCH_NATIVE "disable -march=native flag" OFF)
OPTION(XWREN_DISABLE_TUNE_GENERIC "disable -mtune=generic flag" OFF)

option(XWREN_BUILD_STATIC "Build xeus-wren static library" ON)
OPTION(XWREN_BUILD_SHARED "Split xwren build into executable and library" ON)
OPTION(XWREN_BUILD_XWREN_EXECUTABLE "Build the xwren executable" ON)

OPTION(XWREN_USE_SHARED_XEUS "Link xwren  with the xeus shared library (instead of the static library)" ON)
OPTION(XWREN_USE_SHARED_XEUS_WREN "Link xwren  with the xeus shared library (instead of the static library)" ON)

# Test options
OPTION(XWREN_BUILD_TESTS "xeus-wren test suite" OFF)
OPTION(XWREN_DOWNLOAD_GTEST "build gtest from downloaded sources" OFF)



if(EMSCRIPTEN)
    message("Build with emscripten")
    SET(XWREN_DISABLE_ARCH_NATIVE ON)
    SET(XWREN_DISABLE_TUNE_GENERIC ON)
    SET(XWREN_BUILD_STATIC ON)
    SET(XWREN_BUILD_SHARED OFF)
    SET(XWREN_BUILD_XWREN_EXECUTABLE OFF)
    SET(XWREN_USE_SHARED_XEUS OFF)
    SET(XWREN_USE_SHARED_XEUS_WREN OFF)
    SET(XWREN_BUILD_TESTS OFF)
    SET(XWREN_DOWNLOAD_GTEST OFF)
endif()


# Dependencies
# ============

set(xtl_REQUIRED_VERSION 0.6.23)
set(xeus_REQUIRED_VERSION 0.25.0)

if (NOT TARGET xtl)
    find_package(xtl ${xtl_REQUIRED_VERSION} REQUIRED)
endif ()
if (NOT TARGET xeus AND NOT TARGET xeus-static)
    find_package(xeus ${xeus_REQUIRED_VERSION} REQUIRED)
endif ()



# Flags
# =====

include(CheckCXXCompilerFlag)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4141")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4267 /wd4715 /wd4146 /wd4129")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    if(NOT EMSCRIPTEN)    
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
    endif()
    if (XWREN_DISABLE_ARCH_NATIVE AND NOT XWREN_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic")
    elseif (XWREN_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif ()

    CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)

    if (HAS_CPP14_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else ()
        message(FATAL_ERROR "Unsupported compiler -- xeus requires C++14 support!")
    endif ()
endif ()



if(EMSCRIPTEN)

    add_compile_definitions(EMSCRIPTEN)
    add_compile_definitions(EMSCRIPTEN_ZMQ_REPLACEMENT)

    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} --bind --std=c++17")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -Wno-unused-parameter -Wno-deprecated -Wno-unused-command-line-argument")
    #set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -lwebsocket.js")
    # module / import 
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s MODULARIZE=1 -s EXPORT_NAME=\"createXeusLuaModule\" -s EXPORT_ES6=0 -s USE_ES6_IMPORT_META=0")
    # safty
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES}  -s  DEMANGLE_SUPPORT=1 -s SAFE_HEAP=0  -s ASSERTIONS=2   -s STACK_OVERFLOW_CHECK=1 ")
    # web
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s ENVIRONMENT=web,worker")
    # mem
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s TOTAL_MEMORY=512MB")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s EXIT_RUNTIME=1")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s WASM=1")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s ERROR_ON_UNDEFINED_SYMBOLS=1")

    # threads
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s USE_PTHREADS=1")

    # fetch related
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES}")

    # exceptions    
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s DISABLE_EXCEPTION_CATCHING=0 ")

    # async
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s ASYNCIFY=1 -s 'ASYNCIFY_IMPORTS=[\"async_get_input_function\"]'")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s 'ASYNCIFY_STACK_SIZE=20480'")

    # ENV (https://github.com/emscripten-core/emscripten/commit/6d9681ad04f60b41ef6345ab06c29bbc9eeb84e0)
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=[ENV']\"")

    # filesystem
    file(COPY asset_dir DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} --embed-file asset_dir")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FEATURES}")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}  ${EMSCRIPTEN_FEATURES} ---proxy-to-worker")
endif()

# Source files
# ============

set(XEUS_WREN_SRC
    external/wren/src/vm/wren_compiler.c
    external/wren/src/vm/wren_primitive.c
    external/wren/src/vm/wren_vm.c
    external/wren/src/vm/wren_core.c
    external/wren/src/vm/wren_utils.c
    external/wren/src/vm/wren_debug.c
    external/wren/src/vm/wren_value.c
    external/wren/src/optional/wren_opt_meta.c
    external/wren/src/optional/wren_opt_random.c
    src/xinterpreter.cpp
)

set(WREN_INCLUDE_DIR external/wren/src/include/)
set(WREN_SRC_DIR external/wren/src/vm/)
set(WREN_OPT_SRC_DIR external/wren/src/optional/)

set(XEUS_WREN_HEADERS
    external/wren/src/include/wren.hpp
    external/wren/src/include/wren.h
    include/xeus-wren/xeus_wren_config.hpp
    include/xeus-wren/xinterpreter.hpp
)

set(XWREN_SRC
    src/main.cpp
)



# Targets and link - Macros
# =========================

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

macro(xwren_set_common_options target_name)
    if (MSVC)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4141)
        target_compile_options(${target_name} PUBLIC /wd4018 /wd4267 /wd4715 /wd4146 /wd4129)
    endif ()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")

        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)

        # Mtune generic/native
        if (XWREN_DISABLE_ARCH_NATIVE AND NOT XWREN_DISABLE_TUNE_GENERIC)
            target_compile_options(${target_name} PUBLIC -mtune=generic)
        elseif (XWREN_DISABLE_TUNE_GENERIC)
        else ()
            target_compile_options(${target_name} PUBLIC -march=native)
        endif ()

        # C++14 flag
        CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
        if (HAS_CPP14_FLAG)
            target_compile_features(${target_name} PRIVATE cxx_std_14)
        else ()
            message(FATAL_ERROR "Unsupported compiler -- xeus-wren requires C++14 support!")
        endif ()
    endif ()

    if (APPLE)
        set_target_properties(${target_name} PROPERTIES
            MACOSX_RPATH ON
        )
    else ()
        set_target_properties(${target_name} PROPERTIES
            BUILD_WITH_INSTALL_RPATH 1
            SKIP_BUILD_RPATH FALSE
        )
    endif ()

    set_target_properties(${target_name} PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
endmacro()

# Common macro kernels (xwren )
macro(xwren_set_kernel_options target_name)
   

    if (XWREN_USE_SHARED_XEUS_WREN)
        target_link_libraries(${target_name} PRIVATE xeus-wren)

        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else ()
        target_link_libraries(${target_name} PRIVATE xeus-wren-static)
    endif()

    find_package(Threads) # TODO: add Threads as a dependence of xeus or xeus-static?
    target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endmacro()

# Common macro for shared and static library xeus-wren
macro(xwren_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    add_library(${target_name} ${linkage_upper} ${XEUS_WREN_SRC} ${XEUS_WREN_HEADERS})
    xwren_set_common_options(${target_name})

    set_target_properties(${target_name} PROPERTIES
                          PUBLIC_HEADER "${XEUS_WREN_HEADERS}"
                          PREFIX ""
                          VERSION ${${PROJECT_NAME}_VERSION}
                          SOVERSION ${XWREN_VERSION_MAJOR}
                          OUTPUT_NAME "lib${output_name}")

    target_compile_definitions(${target_name} PUBLIC "XEUS_WREN_EXPORTS")

    target_compile_features(${target_name} PRIVATE cxx_std_17)

    target_include_directories(${target_name}
                               PUBLIC
                               ${PYTHON_INCLUDE_DIRS}
                               $<BUILD_INTERFACE:${XEUS_WREN_INCLUDE_DIR};${WREN_INCLUDE_DIR};${WREN_SRC_DIR};${WREN_OPT_SRC_DIR}>
                               $<INSTALL_INTERFACE:include>)

    if (XWREN_USE_SHARED_XEUS)
        set(XWREN_XEUS_TARGET xeus)
    else ()
        set(XWREN_XEUS_TARGET xeus-static)
    endif ()

    target_link_libraries(${target_name} PUBLIC ${XWREN_XEUS_TARGET} xtl )
    if (WIN32 OR CYGWIN)
        # 
    elseif (APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif ()

    find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
    target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT} )

endmacro()

# xeus-wren
# ===========

set(XEUS_WREN_TARGETS "")

if (XWREN_BUILD_SHARED)
    # Build libraries
    xwren_create_target(xeus-wren SHARED xeus-wren)
    list(APPEND XEUS_WREN_TARGETS xeus-wren)
endif ()

if (XWREN_BUILD_STATIC)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        xwren_create_target(xeus-wren-static STATIC xeus-wren-static)
    else ()
        xwren_create_target(xeus-wren-static STATIC xeus-wren)
    endif ()
    list(APPEND XEUS_WREN_TARGETS xeus-wren-static)
endif ()

# xwren
# =======

if (XWREN_BUILD_XWREN_EXECUTABLE)
    add_executable(xwren ${XWREN_SRC})
    target_compile_features(xwren PRIVATE cxx_std_17)
    xwren_set_common_options(xwren)
    xwren_set_kernel_options(xwren)
endif()




if(EMSCRIPTEN)
    add_executable(xeus_wren src/main_emscripten.cpp )
    target_compile_features(xeus_wren PRIVATE cxx_std_17)
endif()



# Tests
# =====

if(XWREN_DOWNLOAD_GTEST OR XWREN_GTEST_SRC_DIR)
    set(XWREN_BUILD_TESTS ON)
endif()

if(XWREN_BUILD_TESTS)
    add_subdirectory(test)
endif()

# Installation
# ============

include(CMakePackageConfigHelpers)

set(XEUS_WREN_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeus-wrenConfig.cmake")

# Install xeus-wren and xeus-wren-static
if (XWREN_BUILD_SHARED)
    install(TARGETS ${XEUS_WREN_TARGETS}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus-wren)

    # Makes the project importable from the build directory
    export(EXPORT ${PROJECT_NAME}-targets
           FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
endif ()

# Install xwren
if (XWREN_BUILD_XWREN_EXECUTABLE)
    install(TARGETS xwren
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Configuration and data directories for jupyter and xeus-wren
    set(XJUPYTER_DATA_DIR "share/jupyter"    CACHE STRING "Jupyter data directory")

    # Install xwren Jupyter kernelspec
    set(XWREN_KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
    install(DIRECTORY ${XWREN_KERNELSPEC_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE)


    # wren packages (json and iwren)
    set(XWREN_PACKAGES_DIR "asset_dir/wren_packages"    CACHE STRING "Jupyter data directory")
    install(DIRECTORY ${XWREN_PACKAGES_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}/kernels/xwren
            PATTERN "*.in" EXCLUDE)

    # Extra path for installing Jupyter kernelspec
    if (XEXTRA_JUPYTER_DATA_DIR)
        install(DIRECTORY ${XWREN_KERNELSPEC_DIR}
                DESTINATION ${XEXTRA_JUPYTER_DATA_DIR}
                PATTERN "*.in" EXCLUDE)
    endif ()
endif ()

# Configure 'xeus-wrenConfig.cmake' for a build tree
set(XEUS_WREN_CONFIG_CODE "####### Expanded from \@XEUS_WREN_CONFIG_CODE\@ #######\n")
set(XEUS_WREN_CONFIG_CODE "${XEUS_WREN_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set(XEUS_WREN_CONFIG_CODE "${XEUS_WREN_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure 'xeus-wrenConfig.cmake' for an install tree
set(XEUS_WREN_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XEUS_WREN_CMAKECONFIG_INSTALL_DIR})

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION ${XEUS_WREN_CMAKECONFIG_INSTALL_DIR})

if (XWREN_BUILD_SHARED)
    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION ${XEUS_WREN_CMAKECONFIG_INSTALL_DIR})
endif ()
