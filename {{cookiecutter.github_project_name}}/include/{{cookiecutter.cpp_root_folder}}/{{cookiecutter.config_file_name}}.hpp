/***************************************************************************
* Copyright (c) {% now 'utc', '%Y' %}, {{coookiecutter.full_name}}                                  
*                                                                          
* Distributed under the terms of the {{cookiecutter.open_source_license}}.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/
#ifndef {{cookiecutter.cpp_macro_prefix}}_CONFIG_HPP
#define {{cookiecutter.cpp_macro_prefix}}_CONFIG_HPP

// Project version
#define {{cookiecutter.cpp_macro_prefix}}_VERSION_MAJOR 0
#define {{cookiecutter.cpp_macro_prefix}}_VERSION_MINOR 1
#define {{cookiecutter.cpp_macro_prefix}}_VERSION_PATCH 0

// Composing the version string from major, minor and patch
#define {{cookiecutter.cpp_macro_prefix}}_CONCATENATE(A, B) {{cookiecutter.cpp_macro_prefix}}_CONCATENATE_IMPL(A, B)
#define {{cookiecutter.cpp_macro_prefix}}_CONCATENATE_IMPL(A, B) A##B
#define {{cookiecutter.cpp_macro_prefix}}_STRINGIFY(a) {{cookiecutter.cpp_macro_prefix}}_STRINGIFY_IMPL(a)
#define {{cookiecutter.cpp_macro_prefix}}_STRINGIFY_IMPL(a) #a

#define {{cookiecutter.cpp_macro_prefix}}_VERSION {{cookiecutter.cpp_macro_prefix}}_STRINGIFY({{cookiecutter.cpp_macro_prefix}}_CONCATENATE({{cookiecutter.cpp_macro_prefix}}_VERSION_MAJOR,   \
                 {{cookiecutter.cpp_macro_prefix}}_CONCATENATE(.,{{cookiecutter.cpp_macro_prefix}}_CONCATENATE({{cookiecutter.cpp_macro_prefix}}_VERSION_MINOR,   \
                                  {{cookiecutter.cpp_macro_prefix}}_CONCATENATE(.,{{cookiecutter.cpp_macro_prefix}}_VERSION_PATCH)))))

#ifdef _WIN32
    #ifdef {{cookiecutter.cpp_macro_prefix}}_EXPORTS
        #define {{cookiecutter.cpp_macro_prefix}}_API __declspec(dllexport)
    #else
        #define {{cookiecutter.cpp_macro_prefix}}_API __declspec(dllimport)
    #endif
#else
    #define {{cookiecutter.cpp_macro_prefix}}_API
#endif

#endif