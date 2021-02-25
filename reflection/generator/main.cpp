#include "cpp_parser.hpp"
#include "inja.hpp"
#include <nlohmann/json.hpp>
#include <string>

static const std::string module_template_header =
    "#pragma once\n"
    "\n"
    "namespace meta {\n"
    "\n"
    "void InitTypes();\n"
    "\n"
    "}\n";

static const std::string module_template_str =
    "## for header in headers\n"
    "#include \"{{header}}\"\n"
    "## endfor\n"
    "\n"
    "namespace meta {\n"
    "\n"
    "void InitTypes() {\n"
    "\n"
    "    details::RegisterPrimitiveTypes();\n"
    "\n"
    "## for class in classes\n"
    "    details::Reflection<{{class}}>::Allocate();\n"
    "## endfor\n"
    "\n"
    "## for class in classes\n"
    "    details::Reflection<{{class}}>::InitFields();\n"
    "## endfor\n"
    "}\n"
    "\n"
    "}";

static const std::string template_str =
    "#include \"{{header}}\"\n"
    "## for class in classes\n"
    "\n"
    "template<>\n"
    "class ::meta::details::Reflection<{{class.type}}>\n"
    "   : public ::meta::details::TypeInitializer {\n"
    "public:\n"
    "    static void Allocate() {\n"
    "        Register<{{class.type}}>(\"{{class.type}}\");\n"
    "        Register<{{class.type}}*>(\"{{class.type}}\", true);\n"
    "    }\n"
    "\n"
    "    static void InitFields() {\n"
    "## if existsIn(class, \"fields\")\n"
    "## for field in class.fields\n"
    "        AddField<{{class.type}}, {{field.type}}>(\"{{field.name}}\", &get_{{field.name}});\n"
    "## endfor\n"
    "## endif\n"
    "    }\n"
    "## if existsIn(class, \"fields\")\n"
    "## for field in class.fields\n"
    "\n"
    "    static Reference get_{{field.name}}(Reference& instance) {\n"
    "        return Reference(instance.Get<{{class.type}}>().{{field.name}});\n"
    "    }\n"
    "## endfor\n"
    "## endif\n"
    "};\n"
    "## endfor";

static void Generate(const CppParser& parser, const std::string& output) {
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    auto output_path = fs::path(output);
    auto output_directory = output_path.parent_path();
    auto extension = output_path.extension();
    if (extension.empty() || !(extension == ".h" || extension == ".hpp")) {
        output_path.concat(".h");
    }

    json data;
    parser.ForEachClass(
        [&](const CppParser::Class& klass) {
            if (!klass.serialized)
                return;

            json class_data;
            class_data["type"] = klass.name;
            for (auto& field : klass.fields) {
                if (field.specifier == CX_CXXPublic || field.serializable) {
                    class_data["fields"].push_back({
                           { "name", field.name },
                           { "type", field.type_name }
                       });
                }
            }

            data[klass.path]["classes"].emplace_back(std::move(class_data));
        }
    );

    auto inner_output_directory = output_path;
    inner_output_directory.replace_extension("");

    inja::Environment env;
    inja::Template temp = env.parse(template_str.c_str());
    for (auto& [key, value] : data.items()) {
        auto source_path = fs::path(key);
        auto file_path = fs::path(inner_output_directory).append(fs::relative(source_path, fs::current_path()).string());
        auto file_directory = file_path.parent_path();

        value["header"] = fs::relative(source_path, file_directory);

        if (!fs::exists(file_directory)) {
            fs::create_directories(file_directory);
        }

        std::ofstream file(file_path.replace_extension(".gen.h"));
        env.render_to(file, temp, value);
        file.close();
    }

    json module_data;
    module_data["headers"].push_back(output_path.filename());
    for (auto& [key, value] : data.items()) {
        auto inner_path = inner_output_directory;
        auto relative_path = fs::relative(fs::path(key), fs::current_path()).replace_extension("gen.h");
        auto header_path = fs::relative(inner_path.append(relative_path.string()), output_directory);

        module_data["headers"].push_back(header_path.string());
        for (auto& klass : value["classes"]) {
            module_data["classes"].push_back(klass["type"]);
        }
    }

    std::ofstream module_file_header(output_path);
    module_file_header << module_template_header;
    module_file_header.close();

    inja::Template module_template = env.parse(module_template_str.c_str());
    auto output_path_src = output_path;
    output_path_src.replace_extension("cpp");

    std::ofstream module_file_source(output_path_src);
    env.render_to(module_file_source, module_template, module_data);
    module_file_source.close();
}

static std::string ParseCmdLine(int argc, char* argv[], CppParser::Options& opt) {
    using options = CppParser::Options;

    static const char* kArgIncludes = "--includes";
    static const char* kArgImplicitIncludes = "--implicit-includes";
    static const char* kArgSourceFile = "--sources";
    static const char* kArgOutputDirectory = "--output";
    static const char* kArgDefines = "--defines";
    static const char* kArgNameStartSymbols = "--";

    std::string output;
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
    return output;
}

int main(int argc, char *argv[]) {
    std::cout << "parser started...\n";
    CppParser::Options options;
    std::string output = ParseCmdLine(argc, argv, options);

    CppParser parser;
    int error = parser.Parse(options);
    if (error)
        return error;

    parser.ForEachClass(
        [](const CppParser::Class& klass) {
            std::cout << "\n" << klass.name << " (" << klass.path << ")";
            if (klass.serialized) {
                std::cout << " [serialized]";
            }
//            std::cout << "\n-----------------------------------\n";
//            for (const CppParser::Field& field : klass.fields) {
//                std::cout << "    " << field.type_name << " " << field.name << "\n";
//            }
        }
    );

    Generate(parser, output);
    return 0;
}