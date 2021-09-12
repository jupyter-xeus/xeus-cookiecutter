/***************************************************************************
* Copyright (c) {% now 'utc', '%Y' %},                                          
*                                                                          
* Distributed under the terms of the {{cookiecutter.open_source_license}}.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#include <string>
#include <vector>
#include <iostream>

#include "nlohmann/json.hpp"

#include "xeus/xinput.hpp"
#include "xeus/xinterpreter.hpp"

#include "{{cookiecutter.cpp_root_folder}}/xinterpreter.hpp"

namespace nl = nlohmann;

namespace {{cookiecutter.cpp_namespace}}
{
 


    interpreter::interpreter()
    {
        
     
    }

    void interpreter::set_special_functions()
    {

    }

    void interpreter::monkeypatch_io()
    {

       
    }

  

    interpreter::~interpreter()
    {
    }

    void interpreter::configure_impl()
    {
    }

    nl::json interpreter::execute_request_impl(int /*execution_count*/,
                                               const std::string& code,
                                               bool silent,
                                               bool store_history,
                                               nl::json user_expressions,
                                               bool allow_stdin)
    {


        // reset  payload
        nl::json kernel_res;

        //this->m_vm.runFromSource(code);

        kernel_res["payload"] = nl::json::object();
        kernel_res["status"] = "ok";

     

        return kernel_res;
    }

    nl::json interpreter::complete_request_impl(
        const std::string& code,
        int cursor_pos)
    {
        nl::json matches = nl::json::array();


        nl::json result;
        result["status"] = "ok";
        result["matches"] = matches;
        result["cursor_start"] = 0;
        result["cursor_end"] = 0;

        return result;
    }

    nl::json interpreter::inspect_request_impl(const std::string& code,
                                               int cursor_pos,
                                               int detail_level)
    {
        nl::json result;
        result["status"] = "ok";
        result["found"] = true;
        result["data"] = nl::json::object();
        result["metadata"] = nl::json::object();
        return result;    
    }

    nl::json interpreter::is_complete_request_impl(const std::string& code)
    {
        nl::json result;
        result["status"] = code;
        if (code.compare("incomplete") == 0)
        {
            result["indent"] = "   ";
        }
        return result;
    }

    nl::json interpreter::kernel_info_request_impl()
    {
        nl::json result;
        result["implementation"] = "{{cookiecutter.kernel_name}}";
        result["implementation_version"] = {{cookiecutter.cpp_macro_prefix}}_VERSION;
        result["banner"] = "{{cookiecutter.kernel_name}}";
        result["language_info"]["name"] = "{{cookiecutter.language}}";
        result["language_info"]["version"] = "{{cookiecutter.language_version}}";
        result["language_info"]["mimetype"] = "text/x-luasrc";
        result["language_info"]["file_extension"] = "{{cookiecutter.language_file_extension}}";
        return result;
    }

    void interpreter::shutdown_request_impl()
    {
    }

    nl::json interpreter::internal_request_impl(const nl::json& content)
    {
       return nl::json::object();
    }


}
