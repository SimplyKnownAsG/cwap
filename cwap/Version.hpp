#pragma once

#include <string>

namespace cwap {
    /// Contains the cwap version info from Git
    class Version {

    public:
        /// Git SHA1 for this build.
        static const std::string git_sha1;

        /// The result of `git describe --always` for this build.
        static const std::string git_description;
    };
}
