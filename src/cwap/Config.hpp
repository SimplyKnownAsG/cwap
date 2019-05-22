#pragma once

#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "clang-c/Index.h"

using std::string;
using std::unordered_map;
using std::vector;

namespace cwap {

    class Config {
    private:
        friend void update_config(Config*, string);

        vector<string> _files;

        vector<string> _std_inc_dirs;

        unordered_map<string, string> _type_renames;

        Config();

    public:
        // TODO: allow custom config files...
        /* Config(vector<string> files); */

        static Config& GetConfig();

        ~Config() = default;

        std::unordered_map<string, string> const& get_type_renames();

        vector<string> const& get_std_inc_dirs();

        friend std::ostream& operator<<(std::ostream& stream, const Config& self) {
            stream << "<Config ";

            for (auto& fname : self._files) {
                stream << " " << fname;
            }

            stream << ">";
            return stream;
        };
    };
}
