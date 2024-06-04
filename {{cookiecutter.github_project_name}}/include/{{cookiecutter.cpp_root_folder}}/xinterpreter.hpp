/***************************************************************************
* Copyright (c) {% now 'utc', '%Y' %}, {{cookiecutter.full_name}}                                  
*                                                                          
* Distributed under the terms of the {{cookiecutter.open_source_license}}.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/


#ifndef {{cookiecutter.cpp_macro_prefix}}_INTERPRETER_HPP
#define {{cookiecutter.cpp_macro_prefix}}_INTERPRETER_HPP

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

#include <string>
#include <memory>

#include "nlohmann/json.hpp"

#include "{{cookiecutter.config_file_name}}.hpp"
#include "xeus/xinterpreter.hpp"


namespace nl = nlohmann;

namespace {{cookiecutter.cpp_namespace}}
{
    class {{cookiecutter.cpp_macro_prefix}}_API interpreter : public xeus::xinterpreter
    {
    public:

        interpreter();
        virtual ~interpreter() = default;

    protected:

        void configure_impl() override;

        
        void execute_request_impl(send_reply_callback cb,
                                  int execution_counter,
                                  const std::string& code,
                                  xeus::execute_request_config config,
                                  nl::json user_expressions) override;

        nl::json complete_request_impl(const std::string& code, int cursor_pos) override;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;

    };
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

#endif
