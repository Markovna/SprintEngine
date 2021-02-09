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

class CppParser {
public:
    struct Options {
        static const std::string include;
        static const std::string system_include;
        static const std::string define;

        std::string source_file;
        std::vector<std::string> arguments;
    };

    struct Field {
        explicit Field(CXCursor cursor) :
            cursor(cursor),
            specifier(clang_getCXXAccessSpecifier(cursor))
        {
            ExtractString(clang_getTypeSpelling(clang_getCursorType(cursor)), type_name);
            ExtractString(clang_getCursorSpelling(cursor), name);
        }

        CXCursor cursor;
        CX_CXXAccessSpecifier specifier;
        std::string name;
        std::string type_name;
    };

    struct Class {
        explicit Class(CXCursor cursor) :
            cursor(cursor),
            type(clang_getCursorType(cursor))
        {
            CXString name_str = clang_getTypeSpelling(type);
            name = clang_getCString(name_str);
            clang_disposeString(name_str);

            clang_Type_visitFields(
                type,
                [] (CXCursor c, CXClientData client_data) {
                    ((Class*) client_data)->fields.emplace_back(c);
                    return CXVisit_Continue;
                },
                this
            );

            clang_visitChildren(
                cursor,
                [](CXCursor c, CXCursor parent, CXClientData client_data){
                    CXCursorKind kind = clang_getCursorKind(c);
                    if (kind == CXCursor_AnnotateAttr) {
                        if (CompareCXString(clang_getCursorSpelling(c), "__CLASS_SERIALIZED__") == 0) {
                            ((Class*)client_data)->serialized = true;
                            return CXChildVisit_Break;
                        }
                    }
                    return CXChildVisit_Continue;
                },
                this
            );
        }

        CXCursor cursor;
        CXType type;
        std::string name;
        bool serialized = false;
        std::vector<Field> fields;
    };

public:
    int Parse(const Options& options) {
        std::vector<const char *> args;
        for (auto &str : options.arguments) {
            args.push_back(str.c_str());
            std::cout << args.back() << "\n";
        }

        CXIndex index = clang_createIndex(true, true);
        CXTranslationUnit unit;
        CXErrorCode error = clang_parseTranslationUnit2(
            index,
            options.source_file.c_str(),
            args.data(), args.size(),
            nullptr, 0,
            CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_Incomplete,
            &unit
        );

        bool succeed = error == CXError_Success;
        if (succeed) {
            CXCursor cursor = clang_getTranslationUnitCursor(unit);
            ParseClasses(cursor);
        } else {
            Diagnostic(unit, error);
        }

        clang_disposeTranslationUnit(unit);
        clang_disposeIndex(index);
        return succeed ? 0 : -1;
    }

    [[nodiscard]] const std::vector<Class>& GetClasses() const {
        return classes_;
    }

private:
    static void ExtractString(const CXString& cx_str, std::string& str) {
        str = clang_getCString(cx_str);
        clang_disposeString(cx_str);
    }

    static int CompareCXString(const CXString& cx_str, const char* str) {
        int cmp = strcmp(str, clang_getCString(cx_str));
        clang_disposeString(cx_str);
        return cmp;
    }

    static bool Diagnostic(CXTranslationUnit unit, CXErrorCode error) {
        auto diagnostics = clang_getNumDiagnostics(unit);
        if (diagnostics != 0) {
            std::cerr << "\n> Diagnostics:" << std::endl;
            for (int i = 0; i != diagnostics; ++i) {
                auto diag = clang_getDiagnostic(unit, i);
                std::cerr << ">>> " << clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions())
                          << std::endl;
            }
        }

        if (error != CXError_Success) {
            std::cerr << "Unable to parse translation unit. Quitting." << std::endl;
            return false;
        }

        return true;
    }

    void ParseClasses(CXCursor root) {
        clang_visitChildren(
            root,
            [](CXCursor c, CXCursor parent, CXClientData client_data) {
                CXSourceLocation location = clang_getCursorLocation(c);
                if (!clang_Location_isFromMainFile(location))
                    return CXChildVisit_Continue;

                CXCursorKind kind = clang_getCursorKind(c);
                if (kind == CXCursor_StructDecl || kind == CXCursor_ClassDecl) {
                    ((CppParser*)client_data)->classes_.emplace_back(c);
                }

                return CXChildVisit_Recurse;
            },
            this
        );
    }

private:
    std::vector<Class> classes_;
};

const std::string CppParser::Options::include = "-I";
const std::string CppParser::Options::system_include = "-isystem";
const std::string CppParser::Options::define = "-D";

static CppParser::Options ParseCmdLine(int argc, char* argv[]) {
    using options = CppParser::Options;
    options opt;

    static const char* kArgIncludes = "--includes";
    static const char* kArgImplicitIncludes = "--implicit-includes";
    static const char* kArgSourceFile = "--in-source";
    static const char* kArgDefines = "--defines";
    static const char* kArgNameStartSymbols = "--";

    int idx = 0;
    while (idx < argc) {
        char* arg = argv[idx];
        if (strcmp(kArgIncludes, arg) == 0) {
            ++idx;
            while (idx < argc && strncmp(argv[idx], kArgNameStartSymbols, 2) != 0) {
                opt.arguments.emplace_back(options::include + argv[idx]);
                ++idx;
            }
        } else if (strcmp(kArgImplicitIncludes, arg) == 0) {
            ++idx;
            while (idx < argc && strncmp(argv[idx], kArgNameStartSymbols, 2) != 0) {
                char* token = std::strtok(argv[idx], " ");
                while (token) {
                    opt.arguments.emplace_back(options::system_include + token);
                    token = std::strtok(nullptr, " ");
                }
                ++idx;
            }
        } else if (strcmp(kArgDefines, arg) == 0) {
            ++idx;
            while (idx < argc && strncmp(argv[idx], kArgNameStartSymbols, 2) != 0) {
                opt.arguments.emplace_back(options::define + argv[idx]);
                ++idx;
            }
        } else if (strcmp(kArgSourceFile, arg) == 0) {
            opt.source_file = argv[++idx];
            ++idx;
        } else {
            ++idx;
        }
    }

    opt.arguments.emplace_back(options::define + "__CPP_PARSER_META__");
    opt.arguments.emplace_back("-x");
    opt.arguments.emplace_back("c++-header");
    opt.arguments.emplace_back("-std=c++17");
    opt.arguments.emplace_back("-g");
    return opt;
}

int main(int argc, char *argv[]) {
    std::cout << "parser started...\n";
    CppParser parser;
    int error = parser.Parse(ParseCmdLine(argc, argv));
    if (!error) {
        for (const CppParser::Class& klass : parser.GetClasses()) {
            std::cout << "================================= \n" << klass.name;
            if (klass.serialized) {
                std::cout << " (serialized)";
            }
            std::cout << "\n";

            for (const CppParser::Field& field : klass.fields) {
                std::cout << "     " << field.type_name << " " << field.name << "\n";
            }
        }
    }
    return error;
}