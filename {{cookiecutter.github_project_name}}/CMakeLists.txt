#############################################################################
#Copyright (c) {% now 'utc', '%Y' %},                                          
#                                                                         
#Distributed under the terms of the {{cookiecutter.open_source_license}}.                 
#                                                                         
#The full license is in the file LICENSE, distributed with this software. 
#############################################################################

cmake_minimum_required(VERSION 3.4.3)
project({{cookiecutter.project_name}})
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
message(STATUS "Building {{cookiecutter.package_name}} v${${PROJECT_NAME}_VERSION}")

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

option({{cookiecutter.cmake_var_prefix}}_BUILD_STATIC "Build {{cookiecutter.project_name}} static library" ON)
OPTION({{cookiecutter.cmake_var_prefix}}_BUILD_SHARED "Split {{cookiecutter.binary_name}} build into executable and library" ON)
OPTION({{cookiecutter.cmake_var_prefix}}_BUILD_EXECUTABLE "Build the {{cookiecutter.binary_name}} executable" ON)

OPTION({{cookiecutter.cmake_var_prefix}}_USE_SHARED_XEUS "Link {{cookiecutter.binary_name}}  with the xeus shared library (instead of the static library)" ON)
OPTION({{cookiecutter.cmake_var_prefix}}_USE_SHARED_{{cookiecutter.cmake_var_prefix}} "Link {{cookiecutter.binary_name}}  with the xeus shared library (instead of the static library)" ON)


{% if cookiecutter.with_wasm == "yes" -%}
if(EMSCRIPTEN)
    add_compile_definitions({{cookiecutter.cpp_macro_prefix}}_EMSCRIPTEN_WASM_BUILD)
    message("Build with emscripten")
    SET({{cookiecutter.cmake_var_prefix}}_BUILD_STATIC ON)
    SET({{cookiecutter.cmake_var_prefix}}_BUILD_SHARED OFF)
    SET({{cookiecutter.cmake_var_prefix}}_BUILD_EXECUTABLE OFF)
    SET({{cookiecutter.cmake_var_prefix}}_USE_SHARED_XEUS OFF)
    SET({{cookiecutter.cmake_var_prefix}}_USE_SHARED_{{cookiecutter.cmake_var_prefix}} OFF)
    SET({{cookiecutter.cmake_var_prefix}}_BUILD_TESTS OFF)
endif()
{% endif %}

# Dependencies
# ============

set(xtl_REQUIRED_VERSION 0.7.0)
set(xeus_REQUIRED_VERSION 3.0.5)

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
    {% if cookiecutter.with_wasm == "yes" -%}
    if(NOT EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
    endif()
    {% else -%}
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
    {% endif %}

    CHECK_CXX_COMPILER_FLAG("-std=c++{{cookiecutter.cpp_standard}}" HAS_CPP_{{cookiecutter.cpp_standard}}_FLAG)
    if (HAS_CPP_{{cookiecutter.cpp_standard}}_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++{{cookiecutter.cpp_standard}}")
    else ()
        message(FATAL_ERROR "Unsupported compiler -- xeus requires C++{{cookiecutter.cpp_standard}} support!")
    endif ()
endif ()

{% if cookiecutter.with_wasm == "yes" -%}
if(EMSCRIPTEN)
    # ENV (https://github.com/emscripten-core/emscripten/commit/6d9681ad04f60b41ef6345ab06c29bbc9eeb84e0)
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=[ENV']\"")
endif()
{% endif %}

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

        # C++{{cookiecutter.cpp_standard}} flag
        CHECK_CXX_COMPILER_FLAG("-std=c++{{cookiecutter.cpp_standard}}" HAS_CPP_{{cookiecutter.cpp_standard}}_FLAG)
        if (HAS_CPP_{{cookiecutter.cpp_standard}}_FLAG)
            target_compile_features(${target_name} PRIVATE cxx_std_{{cookiecutter.cpp_standard}})
        else ()
            message(FATAL_ERROR "Unsupported compiler -- {{cookiecutter.project_name}} requires C++{{cookiecutter.cpp_standard}} support!")
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

# Common macro kernels ({{cookiecutter.kernel_name}})
macro({{cookiecutter.cmake_var_prefix|lower}}_set_kernel_options target_name)
    if ({{cookiecutter.cmake_var_prefix}}_USE_SHARED_{{cookiecutter.cmake_var_prefix}})
        target_link_libraries(${target_name} PRIVATE {{cookiecutter.library_name}})
        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else ()
        target_link_libraries(${target_name} PRIVATE {{cookiecutter.library_name}}-static)
    endif()

    {% if cookiecutter.with_wasm == "yes" -%}
    if(NOT EMSCRIPTEN)
        find_package(Threads)
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    endif()
    {% else -%}
        find_package(Threads) 
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    {% endif %}
endmacro()

# Common macro for shared and static library
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

    target_compile_features(${target_name} PRIVATE cxx_std_{{cookiecutter.cpp_standard}})

    target_include_directories(${target_name}
                               PUBLIC
                               $<BUILD_INTERFACE:{{'${'}}{{cookiecutter.cmake_var_prefix}}_INCLUDE_DIR}>
                               $<INSTALL_INTERFACE:include>)

    if ({{cookiecutter.cmake_var_prefix}}_USE_SHARED_XEUS)
        set({{cookiecutter.cmake_var_prefix}}_XEUS_TARGET xeus)
    else ()
        set({{cookiecutter.cmake_var_prefix}}_XEUS_TARGET xeus-static)
    endif ()

    target_link_libraries(${target_name} PUBLIC {{'${'}}{{cookiecutter.cmake_var_prefix}}_XEUS_TARGET} xtl)
    if (WIN32 OR CYGWIN)
        #
    elseif (APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif ()
    {% if cookiecutter.with_wasm == "yes" -%}
    if(NOT EMSCRIPTEN)
        find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    endif()
    {% else -%}
        find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    {% endif %}
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

# {{cookiecutter.binary_name}}
# =======
if ({{cookiecutter.cmake_var_prefix}}_BUILD_EXECUTABLE)
    find_package(xeus-zmq 1.0.2 REQUIRED)
    add_executable({{cookiecutter.binary_name}} {{'${'}}{{cookiecutter.cmake_var_prefix}}_MAIN_SRC})
    target_compile_features({{cookiecutter.binary_name}} PRIVATE cxx_std_{{cookiecutter.cpp_standard}})
    {{cookiecutter.cmake_var_prefix|lower}}_set_common_options({{cookiecutter.binary_name}})
    {{cookiecutter.cmake_var_prefix|lower}}_set_kernel_options({{cookiecutter.binary_name}})
    target_link_libraries({{cookiecutter.binary_name}} PRIVATE xeus-zmq)
endif()

{% if cookiecutter.with_wasm == "yes" -%}
if(EMSCRIPTEN)
    include(WasmBuildOptions)
    find_package(xeus-lite REQUIRED)
    add_executable({{cookiecutter.binary_name}} src/main_emscripten_kernel.cpp )
    target_link_libraries({{cookiecutter.binary_name}} PRIVATE xeus-lite)
    {{cookiecutter.cmake_var_prefix}}_set_kernel_options({{cookiecutter.binary_name}})
    xeus_wasm_compile_options({{cookiecutter.binary_name}})
    xeus_wasm_link_options({{cookiecutter.binary_name}} "web,worker")
endif()
{% endif %}

# Installation
# ============
include(CMakePackageConfigHelpers)

set({{cookiecutter.cmake_var_prefix}}_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for {{cookiecutter.project_name}}Config.cmake")

# Install {{cookiecutter.library_name}} and {{cookiecutter.library_name}}-static
if ({{cookiecutter.cmake_var_prefix}}_BUILD_SHARED)
    install(TARGETS {{'${'}}{{cookiecutter.cmake_var_prefix}}_TARGETS}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/{{cookiecutter.cpp_root_folder}})

    # Makes the project importable from the build directory
    export(EXPORT ${PROJECT_NAME}-targets
           FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
endif ()

# Install {{cookiecutter.binary_name}}
if ({{cookiecutter.cmake_var_prefix}}_BUILD_EXECUTABLE)
    install(TARGETS {{cookiecutter.binary_name}}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if ({{cookiecutter.cmake_var_prefix}}_BUILD_EXECUTABLE OR EMSCRIPTEN)
    # Configuration and data directories for jupyter and {{cookiecutter.project_name}}
    set(XJUPYTER_DATA_DIR "share/jupyter"    CACHE STRING "Jupyter data directory")

    # Install {{cookiecutter.kernel_name}} Jupyter kernelspec
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

# Configure '{{cookiecutter.project_name}}Config.cmake' for a build tree
set({{cookiecutter.cmake_var_prefix}}_CONFIG_CODE "####### Expanded from \@{{cookiecutter.cmake_var_prefix}}_CONFIG_CODE\@ #######\n")
set({{cookiecutter.cmake_var_prefix}}_CONFIG_CODE "{{'${'}}{{cookiecutter.cmake_var_prefix}}_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set({{cookiecutter.cmake_var_prefix}}_CONFIG_CODE "{{'${'}}{{cookiecutter.cmake_var_prefix}}_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure '{{cookiecutter.project_name}}Config.cmake.in for an install tree
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

{% if cookiecutter.with_wasm == "yes" -%}
if(EMSCRIPTEN)
    install(TARGETS {{cookiecutter.binary_name}}
            ARCHIVE DESTINATION {{'${'}}{{cookiecutter.cmake_var_prefix}}_CMAKECONFIG_INSTALL_DIR})

    install(FILES
            "$<TARGET_FILE_DIR:{{cookiecutter.binary_name}}>/{{cookiecutter.binary_name}}.js"
            "$<TARGET_FILE_DIR:{{cookiecutter.binary_name}}>/{{cookiecutter.binary_name}}.wasm"
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()
{% endif %}
