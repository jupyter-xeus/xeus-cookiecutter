/***************************************************************************
* Copyright (c) {% now 'utc', '%Y' %}, {{cookiecutter
.full_name}}                                  
*                                                                          
* Distributed under the terms of the {{cookiecutter.open_source_license}}.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#include <iostream>
#include <memory>


#include <emscripten/bind.h>

#include "{{cookiecutter.cpp_root_folder}}/xinterpreter.hpp"
#include "xeus/xembind.hpp"


EMSCRIPTEN_BINDINGS(my_module) {
    xeus::export_core();
    using interpreter_type = {{cookiecutter.cpp_namespace}}::interpreter;
    xeus::export_kernel<interpreter_type>("xkernel");
}

