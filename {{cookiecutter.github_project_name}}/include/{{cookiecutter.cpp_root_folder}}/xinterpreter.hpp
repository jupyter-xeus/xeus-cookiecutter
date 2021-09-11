/***************************************************************************
* Copyright (c) {% now 'utc', '%Y' %},                                          
*                                                                          
* Distributed under the terms of the {{cookiecutter.open_source_license}}.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/


#ifndef XWREH_INTERPRETER_HPP
#define XWREH_INTERPRETER_HPP

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
        virtual ~interpreter();

    protected:

        void configure_impl() override;

        nl::json execute_request_impl(int execution_counter,
                                      const std::string& code,
                                      bool silent,
                                      bool store_history,
                                      nl::json user_expressions,
                                      bool allow_stdin) override;

        nl::json complete_request_impl(const std::string& code, int cursor_pos) override;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;

        nl::json internal_request_impl(const nl::json& content) override;

        void redirect_output();
    private:

        void monkeypatch_io();
        void set_package_path();
        void set_special_functions();

        int complete(const char * code, int cursor_pos, nl::json & matches);
        int get_path(const char * path, int path_length);
        int table_matches(int table_index, const char * identifier, int identifier_length, nl::json & matches);
        int get_metaindex();
    };
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

#endif
