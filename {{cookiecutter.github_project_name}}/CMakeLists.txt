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
project({{cookiecutter.cmake_project_name}})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set({{cookiecutter.cmake_var_prefix}}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(STRINGS "{{'${'}}{{cookiecutter.cmake_var_prefix}}_INCLUDE_DIR}/{{cookiecutter.cpp_root_folder}}/{{cookiecutter.config_file_name}}.hpp" version_defines
     REGEX "#define {{cookiecutter.cpp_macro_prefix}}_VERSION_(MAJOR|MINOR|PATCH)")
foreach (ver ${version_defines})
    if (ver MATCHES "#define {{cookiecutter.cpp_macro_prefix}}_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set({{cookiecutter.cpp_macro_prefix}}_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif ()
endforeach ()
set(${PROJECT_NAME}_VERSION
    {{'${'}}{{cookiecutter.cpp_macro_prefix}}_VERSION_MAJOR}.{{'${'}}{{cookiecutter.cpp_macro_prefix}}_VERSION_MINOR}.{{'${'}}{{cookiecutter.cpp_macro_prefix}}_VERSION_PATCH})
message(STATUS "Building xeus-wren v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

if (NOT DEFINED {{cookiecutter.cmake_var_prefix}}_KERNELSPEC_PATH)
    set({{cookiecutter.cmake_var_prefix}}_KERNELSPEC_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/")
endif ()



configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/{{cookiecutter.kernel_name}}/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/{{cookiecutter.kernel_name}}/kernel.json"
)

# Build options
# =============

# Compilation options
OPTION({{cookiecutter.cmake_var_prefix}}_DISABLE_ARCH_NATIVE "disable -march=native flag" OFF)
OPTION({{cookiecutter.cmake_var_prefix}}_DISABLE_TUNE_GENERIC "disable -mtune=generic flag" OFF)

option({{cookiecutter.cmake_var_prefix}}_BUILD_STATIC "Build {{cookiecutter.cmake_project_name}} static library" ON)
OPTION({{cookiecutter.cmake_var_prefix}}_BUILD_SHARED "Split {{cookiecutter.binary_name}} build into executable and library" ON)
OPTION({{cookiecutter.cmake_var_prefix}}_BUILD_EXECUTABLE "Build the {{cookiecutter.binary_name}} executable" ON)

OPTION({{cookiecutter.cmake_var_prefix}}_USE_SHARED_XEUS "Link {{cookiecutter.binary_name}}  with the xeus shared library (instead of the static library)" ON)
OPTION({{cookiecutter.cmake_var_prefix}}_USE_SHARED_{{cookiecutter.cmake_var_prefix}} "Link {{cookiecutter.binary_name}}  with the xeus shared library (instead of the static library)" ON)


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
    if ({{cookiecutter.cmake_var_prefix}}_DISABLE_ARCH_NATIVE AND NOT {{cookiecutter.cmake_var_prefix}}_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic")
    elseif ({{cookiecutter.cmake_var_prefix}}_DISABLE_TUNE_GENERIC)
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



    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FEATURES}")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}  ${EMSCRIPTEN_FEATURES} ---proxy-to-worker")
endif()

# Source files
# ============

set({{cookiecutter.cmake_var_prefix}}_HEADERS
    include/{{cookiecutter.cpp_root_folder}}/{{cookiecutter.config_file_name}}.hpp
    include/{{cookiecutter.cpp_root_folder}}/xinterpreter.hpp
)

set({{cookiecutter.cmake_var_prefix}}_SRC
    src/xinterpreter.cpp 
)

set({{cookiecutter.cmake_var_prefix}}_MAIN_SRC
    src/main.cpp
)



# Targets and link - Macros
# =========================

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

macro({{cookiecutter.cmake_var_prefix|lower}}_set_common_options target_name)
    if (MSVC)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4141)
        target_compile_options(${target_name} PUBLIC /wd4018 /wd4267 /wd4715 /wd4146 /wd4129)
    endif ()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")

        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)

        # Mtune generic/native
        if ({{cookiecutter.cmake_var_prefix}}_DISABLE_ARCH_NATIVE AND NOT {{cookiecutter.cmake_var_prefix}}_DISABLE_TUNE_GENERIC)
            target_compile_options(${target_name} PUBLIC -mtune=generic)
        elseif ({{cookiecutter.cmake_var_prefix}}_DISABLE_TUNE_GENERIC)
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
macro({{cookiecutter.cmake_var_prefix|lower}}_set_kernel_options target_name)
   

    if ({{cookiecutter.cmake_var_prefix}}_USE_SHARED_{{cookiecutter.cmake_var_prefix}})
        target_link_libraries(${target_name} PRIVATE {{cookiecutter.library_name}})

        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else ()
        target_link_libraries(${target_name} PRIVATE {{cookiecutter.library_name}}-static)
    endif()

    find_package(Threads) # TODO: add Threads as a dependence of xeus or xeus-static?
    target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endmacro()

# Common macro for shared and static library xeus-wren
macro({{cookiecutter.cmake_var_prefix|lower}}_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    add_library(${target_name} ${linkage_upper} {{'${'}}{{cookiecutter.cmake_var_prefix}}_SRC} {{'${'}}{{cookiecutter.cmake_var_prefix}}_HEADERS})
    {{cookiecutter.cmake_var_prefix|lower}}_set_common_options(${target_name})

    set_target_properties(${target_name} PROPERTIES
                          PUBLIC_HEADER "{{'${'}}{{cookiecutter.cmake_var_prefix}}_HEADERS}"
                          PREFIX ""
                          VERSION ${${PROJECT_NAME}_VERSION}
                          SOVERSION {{'${'}}{{cookiecutter.cpp_macro_prefix}}_VERSION_MAJOR}
                          OUTPUT_NAME "lib${output_name}")

    target_compile_definitions(${target_name} PUBLIC "{{cookiecutter.cpp_macro_prefix}}_EXPORTS")

    target_compile_features(${target_name} PRIVATE cxx_std_17)

    target_include_directories(${target_name}
                               PUBLIC
                               $<BUILD_INTERFACE:{{'${'}}{{cookiecutter.cmake_var_prefix}}_INCLUDE_DIR}>
                               $<INSTALL_INTERFACE:include>)

    if ({{cookiecutter.cmake_var_prefix}}_USE_SHARED_XEUS)
        set({{cookiecutter.cmake_var_prefix}}_XEUS_TARGET xeus)
    else ()
        set({{cookiecutter.cmake_var_prefix}}_XEUS_TARGET xeus-static)
    endif ()

    target_link_libraries(${target_name} PUBLIC {{'${'}}{{cookiecutter.cmake_var_prefix}}_XEUS_TARGET} xtl )
    if (WIN32 OR CYGWIN)
        # 
    elseif (APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif ()

    find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
    target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT} )

endmacro()

# {{cookiecutter.library_name}}
# ===========

set({{cookiecutter.cmake_var_prefix}}_TARGETS "")

if ({{cookiecutter.cmake_var_prefix}}_BUILD_SHARED)
    # Build libraries
    {{cookiecutter.cmake_var_prefix|lower}}_create_target({{cookiecutter.library_name}} SHARED {{cookiecutter.library_name}})
    list(APPEND {{cookiecutter.cmake_var_prefix}}_TARGETS {{cookiecutter.library_name}})
endif ()

if ({{cookiecutter.cmake_var_prefix}}_BUILD_STATIC)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        {{cookiecutter.cmake_var_prefix|lower}}_create_target({{cookiecutter.library_name}}-static STATIC {{cookiecutter.library_name}}-static)
    else ()
        {{cookiecutter.cmake_var_prefix|lower}}_create_target({{cookiecutter.library_name}}-static STATIC {{cookiecutter.library_name}})
    endif ()
    list(APPEND {{cookiecutter.cmake_var_prefix}}_TARGETS {{cookiecutter.library_name}}-static)
endif ()

# {{cookiecutter.library_name}}
# =======
if ({{cookiecutter.cmake_var_prefix}}_BUILD_EXECUTABLE)
    add_executable({{cookiecutter.binary_name}} {{'${'}}{{cookiecutter.cmake_var_prefix}}_MAIN_SRC})
    target_compile_features({{cookiecutter.binary_name}} PRIVATE cxx_std_17)
    {{cookiecutter.cmake_var_prefix|lower}}_set_common_options({{cookiecutter.binary_name}})
    {{cookiecutter.cmake_var_prefix|lower}}_set_kernel_options({{cookiecutter.binary_name}})
endif()


# Installation
# ============

include(CMakePackageConfigHelpers)

set({{cookiecutter.cmake_var_prefix}}_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeus-wrenConfig.cmake")

# Install {{cookiecutter.library_name}} and {{cookiecutter.library_name}}-static
if ({{cookiecutter.cmake_var_prefix}}_BUILD_SHARED)
    install(TARGETS {{'${'}}{{cookiecutter.cmake_var_prefix}}_TARGETS}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus-wren)

    # Makes the project importable from the build directory
    export(EXPORT ${PROJECT_NAME}-targets
           FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
endif ()

# Install {{cookiecutter.binary_name}}
if ({{cookiecutter.cmake_var_prefix}}_BUILD_EXECUTABLE)
    install(TARGETS {{cookiecutter.binary_name}}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Configuration and data directories for jupyter and xeus-wren
    set(XJUPYTER_DATA_DIR "share/jupyter"    CACHE STRING "Jupyter data directory")

    # Install xwren Jupyter kernelspec
    set(KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
    install(DIRECTORY ${KERNELSPEC_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE)


    # Extra path for installing Jupyter kernelspec
    if (XEXTRA_JUPYTER_DATA_DIR)
        install(DIRECTORY ${KERNELSPEC_DIR}
                DESTINATION ${XEXTRA_JUPYTER_DATA_DIR}
                PATTERN "*.in" EXCLUDE)
    endif ()
endif ()

# Configure 'xeus-wrenConfig.cmake' for a build tree
set({{cookiecutter.cmake_var_prefix}}_CONFIG_CODE "####### Expanded from \@{{cookiecutter.cmake_var_prefix}}_CONFIG_CODE\@ #######\n")
set({{cookiecutter.cmake_var_prefix}}_CONFIG_CODE "{{'${'}}{{cookiecutter.cmake_var_prefix}}_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set({{cookiecutter.cmake_var_prefix}}_CONFIG_CODE "{{'${'}}{{cookiecutter.cmake_var_prefix}}_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure '{{cookiecutter.cmake_project_name}}Config.cmake.in for an install tree
set({{cookiecutter.cmake_var_prefix}}_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION {{'${'}}{{cookiecutter.cmake_var_prefix}}_CMAKECONFIG_INSTALL_DIR})

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION {{'${'}}{{cookiecutter.cmake_var_prefix}}_CMAKECONFIG_INSTALL_DIR})

if ({{cookiecutter.cmake_var_prefix}}_BUILD_SHARED)
    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION {{'${'}}{{cookiecutter.cmake_var_prefix}}_CMAKECONFIG_INSTALL_DIR})
endif ()
