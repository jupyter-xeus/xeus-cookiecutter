/***************************************************************************
* Copyright (c) {% now 'utc', '%Y' %}, {{cookiecutter
.full_name}}                                  
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
        xeus::register_interpreter(this);
    }

    nl::json interpreter::execute_request_impl(int execution_counter, // Typically the cell number
                                                      const std::string& code, // Code to execute
                                                      bool /*silent*/,
                                                      bool /*store_history*/,
                                                      nl::json /*user_expressions*/,
                                                      bool /*allow_stdin*/)
    {
        nl::json kernel_res;

        if (code.compare("hello, world") == 0)
        {
            publish_stream("stdout", code);
        }

        if (code.compare("error") == 0)
        {
            publish_stream("stderr", code);
        }

        if (code.compare("?") == 0)
        {
            std::string html_content = R"(<iframe class="xpyt-iframe-pager" src="
                https://xeus.readthedocs.io"></iframe>)";

            kernel_res["status"] = "ok";
            kernel_res["payload"] = nl::json::array();
            kernel_res["payload"][0] = nl::json::object({
                {"data", {
                    {"text/plain", "https://xeus.readthedocs.io"},
                    {"text/html", html_content}}
                },
                {"source", "page"},
                {"start", 0}
            });
            kernel_res["user_expressions"] = nl::json::object();

            return kernel_res;
        }

        nl::json pub_data;
        pub_data["text/plain"] = code;
        publish_execution_result(execution_counter, std::move(pub_data), nl::json());

        kernel_res["status"] = "ok";
        kernel_res["payload"] = nl::json::array();
        kernel_res["user_expressions"] = nl::json::object();

        return kernel_res;

    }

    void interpreter::configure_impl()
    {
        // Perform some operations
    }

    nl::json interpreter::is_complete_request_impl(const std::string& code)
    {
        nl::json result;
        result["status"] = "complete";
        if (code.compare("incomplete") == 0)
        {
            result["status"] = "incomplete";
            result["indent"] = "   ";
        }
        else if(code.compare("invalid") == 0)
        {
            result["status"] = "invalid";
            result["indent"] = "   ";
        }
        return result;
    }

    nl::json interpreter::complete_request_impl(const std::string&  code,
                                                     int cursor_pos)
    {
        nl::json result;

        // Code starts with 'H', it could be the following completion
        if (code[0] == 'H')
        {
            result["status"] = "ok";
            result["matches"] = {
                std::string("Hello"), 
                std::string("Hey"), 
                std::string("Howdy")
            };
            result["cursor_start"] = 5;
            result["cursor_end"] = cursor_pos;
        }
        // No completion result
        else
        {
            result["status"] = "ok";
            result["matches"] = nl::json::array();
            result["cursor_start"] = cursor_pos;
            result["cursor_end"] = cursor_pos;
        }

        return result;
    }

    nl::json interpreter::inspect_request_impl(const std::string& /*code*/,
                                                      int /*cursor_pos*/,
                                                      int /*detail_level*/)
    {
        nl::json result;
        result["status"] = "ok";
        result["found"] = true;
        {% raw %}
        result["data"] = {{std::string("text/plain"), std::string("hello!")}};
        result["metadata"] = {{std::string("text/plain"), std::string("hello!")}};
        {% endraw %} 
        return result;
    }

    void interpreter::shutdown_request_impl() {
        std::cout << "Bye!!" << std::endl;
    }

    nl::json interpreter::kernel_info_request_impl()
    {
        nl::json result;
        result["implementation"] = "{{cookiecutter.kernel_name}}";
        result["implementation_version"] = {{cookiecutter.cpp_macro_prefix}}_VERSION;
        result["banner"] = "{{cookiecutter.kernel_name}}";
        result["language_info"]["name"] = "{{cookiecutter.language}}";
        result["language_info"]["version"] = "{{cookiecutter.language_version}}";
        result["language_info"]["mimetype"] = "{{cookiecutter.language_mimetype}}";
        result["language_info"]["file_extension"] = "{{cookiecutter.language_file_extension}}";
        return result;
    }

}
