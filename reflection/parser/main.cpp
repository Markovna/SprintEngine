#include "cpp_parser.hpp"
#include "inja.hpp"
#include <nlohmann/json.hpp>
#include <string>

static CppParser::Options ParseCmdLine(int argc, char* argv[], std::string& output) {
    using options = CppParser::Options;
    options opt;

    static const char* kArgIncludes = "--includes";
    static const char* kArgImplicitIncludes = "--implicit-includes";
    static const char* kArgSourceFile = "--sources";
    static const char* kArgOutputDirectory = "--output";
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
            ++idx;
            while (idx < argc && strncmp(argv[idx], kArgNameStartSymbols, 2) != 0) {
                char* token = std::strtok(argv[idx], " ");
                while (token) {
                    opt.source_files.emplace_back(token);
                    token = std::strtok(nullptr, " ");
                }
                ++idx;
            }
        } else if (strcmp(kArgOutputDirectory, arg) == 0) {
            output = argv[++idx];
        }
        else {
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
    std::string output;
    int error = parser.Parse(ParseCmdLine(argc, argv, output));
    if (error)
        return error;

    parser.ForEachClass(
        [](const CppParser::Class& klass) {
            std::cout << "\n" << klass.name << "\n(" << klass.path << ")";
            if (klass.serialized) {
                std::cout << " (serialized)";
            }
            std::cout << "\n-----------------------------------\n";

            for (const CppParser::Field& field : klass.fields) {
                std::cout << "    " << field.type_name << " " << field.name << "\n";
            }
        }
    );

    {
        static const std::stringstream template_stream(
            "#include \"runtime.h\"\n"
            "#include \"{{header}}\"\n"
            "\n"
            "## for class in classes\n"
            "template<>\n"
            "class ::sprint::meta::details::Reflection<{{class.type}}>\n"
            "   : public ::sprint::meta::details::TypeInitializer<{{class.type}}> {\n"
            "public:\n"
            "    Reflection()\n"
            "        : TypeInitializer(\"{{class.type}}\")\n"
            "    {}\n"
            "\n"
            "    void InitFields() {\n"
            "## if existsIn(class, \"fields\")\n"
            "## for field in class.fields\n"
            "        AddField<{{field.type}}>(\"{{field.name}}\");\n"
            "## endfor\n"
            "## endif\n"
            "    }\n"
            "\n"
            "};\n"
            "## endfor");

        nlohmann::json data;
        parser.ForEachClass(
            [&](const CppParser::Class& klass) {
                nlohmann::json class_data;
                class_data["type"] = klass.name;
                for (auto& field : klass.fields) {
                    class_data["fields"].push_back({
                         {"name", field.name},
                         {"type", field.type_name}
                    });
                }

                data[klass.path]["classes"].emplace_back(std::move(class_data));
            }
        );

        namespace fs = std::filesystem;

        inja::Environment env;
        inja::Template temp = env.parse(template_stream.str());
        for (auto& [key, value] : data.items()) {
            auto path = fs::path(key);
            auto output_path = fs::path(output).append(fs::relative(path, fs::current_path()).string());
            auto output_directory = output_path.parent_path();

            value["header"] = fs::relative(path, output_directory);

            if (!fs::exists(output_directory)) {
                fs::create_directories(output_directory);
            }

            std::ofstream file(output_path.replace_extension(".gen.h"));
            env.render_to(file, temp, value);
            file.close();
        }
    }

    return 0;
}