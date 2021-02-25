#pragma once

#include <clang-c/Index.h>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

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

        std::vector<std::string> source_files;
        std::string source_file;
        std::vector<std::string> arguments;
    };

    struct Field {
        explicit Field(CXCursor cursor) :
            cursor(cursor),
            specifier(clang_getCXXAccessSpecifier(cursor))
        {
            ExtractString(clang_getCursorSpelling(cursor), name);
            ExtractString(clang_getTypeSpelling(clang_getCanonicalType(clang_getCursorType(cursor))), type_name);

            clang_visitChildren(
                cursor,
                [](CXCursor c, CXCursor parent, CXClientData client_data){
                    CXCursorKind kind = clang_getCursorKind(c);
                    if (kind == CXCursor_AnnotateAttr) {
                        if (CompareCXString(clang_getCursorSpelling(c), "__FIELD_SERIALIZED__") == 0) {
                            ((Field*)client_data)->serializable = true;
                        }
                    }
                    return CXChildVisit_Continue;
                },
                this
            );
        }

        CXCursor cursor;
        CX_CXXAccessSpecifier specifier;
        std::string name;
        std::string type_name;
        bool serializable = false;
    };

    struct Class {
        explicit Class(CXCursor cursor) :
            cursor(cursor),
            type(clang_getCanonicalType(clang_getCursorType(cursor)))
        {
            ExtractString(clang_getTypeSpelling(type), name);

            CXFile file;
            unsigned line, column, offset;
            clang_getFileLocation(clang_getCursorLocation(cursor), &file, &line, &column, &offset);
            ExtractString(clang_File_tryGetRealPathName(file), path);

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
        std::string path;
        bool serialized = false;
        std::vector<Field> fields;
    };

    static bool CreatePch(std::string header, CXIndex& index, const Options& options, std::string& output) {
        if (header.empty())
            return false;

        std::vector<const char *> args;
        for (auto &str : options.arguments) {
            args.push_back(str.c_str());
        }

        CXTranslationUnit unit;
        CXErrorCode error = clang_parseTranslationUnit2(
            index,
            header.c_str(),
            args.data(), args.size(),
            nullptr, 0,
            CXTranslationUnit_ForSerialization,
            &unit);
        Diagnostic(unit, error);

        namespace fs = std::filesystem;

        output = fs::path(header).filename().replace_extension("pch").string();
        clang_saveTranslationUnit(unit, output.c_str(), clang_defaultSaveOptions(unit));
        clang_disposeTranslationUnit(unit);

        std::cout << "Precompiled header " << output << " was created."<< std::endl;
        return true;
    }

public:
    int Parse(const Options& options) {
        std::vector<const char *> args;
        for (auto &str : options.arguments) {
            args.push_back(str.c_str());
            std::cout << args.back() << "\n";
        }

        CXIndex index = clang_createIndex(true, true);

        CXTranslationUnit unit;
        for (auto& source_file : options.source_files) {
            CXErrorCode error = clang_parseTranslationUnit2(
                index,
                source_file.c_str(),
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
                clang_disposeTranslationUnit(unit);
                return -1;
            }
        }

        clang_disposeTranslationUnit(unit);
        clang_disposeIndex(index);
        return 0;
    }

    template <class F>
    void ForEachClass(F func) const {
        for (const Class& klass : classes_) {
            func(klass);
        }
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

