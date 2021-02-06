#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>

std::ostream &operator<<(std::ostream &stream, const CXString &str) {
    stream << clang_getCString(str);
    clang_disposeString(str);
    return stream;
}

namespace cpp_parser {

struct options {
    std::string src_file;
    std::vector<std::string> arguments;
};

int parse(const options& options) {
    std::cout << "Start parse..." << options.src_file << std::endl;

    std::vector<const char*> args;
    for (auto& arg : options.arguments) {
        args.push_back(arg.c_str());
    }

    CXIndex index = clang_createIndex(0, true);
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index,
        options.src_file.c_str(),
        args.data(), args.size(),
        nullptr, 0,
        CXTranslationUnit_SkipFunctionBodies
    );

    if (unit == nullptr) {
        std::cerr << "Unable to parse translation unit. Quitting." << std::endl;
        return -1;
    }

    CXCursor root_cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(
        root_cursor,
        [] (CXCursor c, CXCursor parent, CXClientData client_data) {
            CXSourceLocation location = clang_getCursorLocation(c);

            if (clang_Location_isFromMainFile(location) == 0)
                return CXChildVisit_Continue;

            CXType type = clang_getCursorType(c);

            std::cout << "Cursor '" << clang_getCursorSpelling(c) << "' of kind '"
                      << clang_getCursorKindSpelling(clang_getCursorKind(c)) << "' of type '"
                      << clang_getTypeSpelling(type) << "'\n";
            return CXChildVisit_Recurse;
        },
        nullptr);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return 0;
}

}

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

int main(int argc, char *argv[]) {
    std::cout << "parser started...\n";
//    for (int i = 0; i < argc; i++) {
//        std::cout << argv[i] << "\n";
//    }
//    std::cout << std::endl;

    {
//        auto source_directory = std::filesystem::path(argv[0]).parent_path();
//        if (!source_directory.empty()) {
//            std::error_code error;
//            std::filesystem::current_path(source_directory, error);
//            if (error) {
//                std::cerr << "Couldn't set current path: " << error.message() << std::endl;
//                return -1;
//            }
//        }

        std::vector<std::string> arguments;
        std::string source;

        // TODO: include precompiled headers (-include-pch)

        arguments.emplace_back("-x");
        arguments.emplace_back("c++-header");
        arguments.emplace_back("-std=c++17");
        arguments.emplace_back("-g");

        std::stringstream ss;

        auto is_command = [] (const char* c) { return c[0] == '-' && c[1] == '-'; };

        for (int ii = 1; ii < argc; ii++) {
            if (std::string("--implicit-includes") == argv[ii-1]) {
                while (ii < argc && !is_command(argv[ii])) {
                    std::istringstream iss(argv[ii]);
                    std::string item;
                    while (std::getline(iss, item, ' ')) {
                        arguments.emplace_back("-isystem" + item);
                    }
                    ii++;
                }
            } else if (std::string("--in-source") == argv[ii-1]) {
                source = argv[ii];
            } else if (std::string("--includes") == argv[ii-1]) {
                while (ii < argc && !is_command(argv[ii])) {
                    std::istringstream iss(argv[ii]);
                    std::string item;
                    while (std::getline(iss, item, ' ')) {
                        arguments.emplace_back("-I" + item);
                    }
                    ii++;
                }
            }
            else if (std::string("--defines") == argv[ii-1]) {
                while (ii < argc && !is_command(argv[ii])) {
                    std::istringstream iss(argv[ii]);
                    std::string item;
                    while (std::getline(iss, item, ' ')) {
                        arguments.emplace_back("-D" + item);
                    }
                    ii++;
                }
            }
        }

        std::cout << "source: " << source << "\n";
        std::cout << "args: \n";

        std::vector<const char*> args;
        for (auto& str : arguments) {
            std::cout << str << "\n";
            args.push_back(str.c_str());
        }
        std::cout << std::endl;

        CXIndex index = clang_createIndex(0, true);
        CXTranslationUnit unit;
        CXErrorCode error = clang_parseTranslationUnit2(
            index,
            source.c_str(),
            args.data(), args.size(),
            nullptr, 0,
            CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_Incomplete,
            &unit
        );

        auto diagnostics = clang_getNumDiagnostics(unit);
        if (diagnostics != 0)
        {
            std::cerr << "\n> Diagnostics:" << std::endl;
            for (int i = 0; i != diagnostics; ++i)
            {
                auto diag = clang_getDiagnostic(unit, i);
                std::cerr << ">>> " << clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions())
                          << std::endl;
            }
        }
        if (error != CXError_Success || unit == nullptr) {
            std::cerr << "Unable to parse translation unit. Quitting." << std::endl;
            return -1;
        } else {
            std::cerr << "Succeed " << source << std::endl;
        }

        if (false)
        {
            CXCursor root_cursor = clang_getTranslationUnitCursor(unit);
            clang_visitChildren(
                root_cursor,
                [](CXCursor c, CXCursor parent, CXClientData client_data) {
                    CXSourceLocation location = clang_getCursorLocation(c);

                    if (clang_Location_isFromMainFile(location) == 0)
                        return CXChildVisit_Continue;

                    CXType type = clang_getCursorType(c);

                    std::cout << "Cursor '" << clang_getCursorSpelling(c) << "' of kind '"
                              << clang_getCursorKindSpelling(clang_getCursorKind(c)) << "' of type '"
                              << clang_getTypeSpelling(type) << "'\n";
                    return CXChildVisit_Recurse;
                },
                nullptr);
        }

        clang_disposeTranslationUnit(unit);
        clang_disposeIndex(index);
        return 0;
    }

    // set the working directory to the executable directory
//    auto source_directory = std::filesystem::path(argv[0]).parent_path();
//    if (!source_directory.empty()) {
//        std::error_code error;
//        std::filesystem::current_path(source_directory, error);
//        if (error) {
//            std::cerr << "Couldn't set current path: " << error.message() << std::endl;
//            return -1;
//        }
//    }

    cpp_parser::options options;
//    options.arguments.emplace_back("-x");
//    options.arguments.emplace_back("c++");
    options.arguments.emplace_back("-g");
    options.arguments.emplace_back("-std=c++14");

    options.src_file = "main.cpp";

//    options.src_file = argv[2];

    return cpp_parser::parse(options);
}