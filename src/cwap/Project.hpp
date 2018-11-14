#pragma once

#include "cwap/ConvenientClang.hpp"
#include "cwap/Namespace.hpp"

#include <clang-c/Index.h>

#include <string>
#include <unordered_map>
#include <unordered_set>

using std::string;

namespace cwap {

    class Project : public Namespace {
    private:
        std::unordered_set<string> _sources;

        friend CXChildVisitResult find_RENAME_THIS(CXCursor cursor,
                                                   CXCursor parent,
                                                   CXClientData client_data);

        class _cacher {
        private:
            /* int indent = -2; */

        public:
            std::unordered_map<string, void*> _cache;
            template<typename T>
            T* get(Project& project, const CXCursor& cursor) {
                string usr = get_usr(cursor);
                T* result = nullptr;

                try {
                    result = (T*)this->_cache.at(usr);
                } catch (std::exception& ex) {
                    result = T::Create(project, cursor);

                    if (result->usr != "") {
                        this->_cache[usr] = result;
                    } else if (this->_cache.count(result->name)) {
                        delete result;
                        result = (T*)this->_cache.at(result->name);
                    } else {
                        this->_cache[result->name] = result;
                    }
                }

                return result;
            };

            Type* get(Project& project, const CXType& cxtype) {
                auto usr = get_usr(clang_getTypeDeclaration(cxtype));
                Type* result = nullptr;
                try {
                    result = (Type*)this->_cache.at(usr);
                } catch (std::exception& ex) {
                    result = Type::Create(project, cxtype);
                    if (result->usr != "") {
                        this->_cache[usr] = result;
                    } else if (this->_cache.count(result->name)) {
                        delete result;
                        result = (Type*)this->_cache.at(result->name);
                    } else {
                        this->_cache[result->name] = result;
                    }
                }

                return result;
            };

        } _cache;

    public:
        Project(string name);

        ~Project() = default;

        std::unordered_map<string, string> type_renames;

        void process_options(string filename);

        const std::unordered_set<string> sources() const;

        void parse(string filename);

        void parse(string filename, std::vector<string> clang_args);

        void parse(std::vector<string> filenames, std::vector<string> clang_args);

        void write_yaml();

        void write_header(std::ostream& stream, std::string indent = "") const override;

        template<typename T>
        T* get(const CXCursor& cursor) {
            return this->_cache.get<T>(*this, cursor);
        };

        Type* get(const CXType& cxtype) {
            return this->_cache.get(*this, cxtype);
        };

        friend std::ostream& operator<<(std::ostream& stream, const Project& self) {
            stream << "<Project " << self.name << ">";
            return stream;
        };
    };
}
