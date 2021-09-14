/***************************************************************************
* Copyright (c) 2021,                                          
*                                                                          
* Distributed under the terms of the MIT license.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#include <string>
#include <vector>
#include <iostream>

#include "nlohmann/json.hpp"

#include "xeus/xinput.hpp"
#include "xeus/xinterpreter.hpp"

#include "xeus-lua/xinterpreter.hpp"

namespace nl = nlohmann;

namespace xeus_lua
{
 


    interpreter::interpreter()
    {
        xeus::register_interpreter(this);
    }

    nl::json interpreter::execute_request_impl(int execution_counter, // Typically the cell number
                                                      const std::string& /*code*/, // Code to execute
                                                      bool /*silent*/,
                                                      bool /*store_history*/,
                                                      nl::json /*user_expressions*/,
                                                      bool /*allow_stdin*/)
    {
        // You can use the C-API of your target language for executing the code,
        // e.g. `PyRun_String` for the Python C-API
        //      `luaL_dostring` for the Lua C-API

        // Use this method for publishing the execution result to the client,
        // this method takes the ``execution_counter`` as first argument,
        // the data to publish (mime type data) as second argument and metadata
        // as third argument.
        // Replace "Hello World !!" by what you want to be displayed under the execution cell
        nl::json pub_data;
        pub_data["text/plain"] = "Hello World !!";
        publish_execution_result(execution_counter, std::move(pub_data), nl::json::object());

        // You can also use this method for publishing errors to the client, if the code
        // failed to execute
        // publish_execution_error(error_name, error_value, error_traceback);
        publish_execution_error("TypeError", "123", {"!@#$", "*(*"});

        nl::json result;
        result["status"] = "ok";
        return result;
    }

    void interpreter::configure_impl()
    {
        // Perform some operations
    }

    nl::json interpreter::complete_request_impl(const std::string& code,
                                                       int cursor_pos)
    {
        nl::json result;

        // Code starts with 'H', it could be the following completion
        if (code[0] == 'H')
        {
            result["status"] = "ok";
            result["matches"] = {"Hello", "Hey", "Howdy"};
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

    nl::json interpreter::inspect_request_impl(const std::string& code,
                                                      int /*cursor_pos*/,
                                                      int /*detail_level*/)
    {
        nl::json result;
        result["data"] = nl::json::object();
        result["metadata"] = nl::json::object();

        if (code.compare("print") == 0)
        {

            result["found"] = true;
            result["text/plain"] = "Print objects to the text stream file, [...]";
        }
        else
        {
            result["found"] = false;
        }

        result["status"] = "ok";
        return result;
    }

   
    void interpreter::shutdown_request_impl() {
        std::cout << "Bye!!" << std::endl;
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
        result["implementation"] = "xlua";
        result["implementation_version"] = XEUS_LUA_VERSION;
        result["banner"] = "xlua";
        result["language_info"]["name"] = "lua";
        result["language_info"]["version"] = "1.0.0";
        result["language_info"]["mimetype"] = "text/x-luasrc";
        result["language_info"]["file_extension"] = "lua";
        return result;
    }

}
