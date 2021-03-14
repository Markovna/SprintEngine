#include "shader_utils.h"

namespace sprint {

namespace gfx {

void PreProcess(
    const std::string &source,
    std::string& vertex_shader,
    std::string& fragment_shader,
    Attribute::BindingPack& bindings) {

    {
        static const std::string typeToken = "#type";
        size_t pos = source.find(typeToken, 0);
        ShaderType::Enum type;

        while (pos != std::string::npos) {
            size_t eol = source.find_first_of('\n', pos);
            size_t begin = pos + typeToken.size() + 1;
            std::string typeStr = source.substr(begin, eol - begin);
            uint64_t nextLinePos = eol + 1;
            pos = source.find(typeToken, nextLinePos);

            if (ShaderType::TryParse(typeStr, type)) {
                std::string str = (pos == std::string::npos) ?
                                  source.substr(nextLinePos) :
                                  source.substr(nextLinePos, pos - nextLinePos);
                if (type == ShaderType::Vertex) {
                    vertex_shader = std::move(str);
                } else if (type == ShaderType::Fragment) {
                    fragment_shader = std::move(str);
                }
            }
        }
    }

    {
        static const std::string token = "#binding";
        size_t pos = vertex_shader.find(token, 0);
        int loc = 0;
        const char* location_template_str = "layout (location = #)";
        const size_t location_str_pos = 19;
        while (pos != std::string::npos) {
            size_t eol = vertex_shader.find_first_of('\n', pos);
            size_t begin = pos + token.size() + 1;
            std::string binding_type = vertex_shader.substr(begin, eol - begin);

            std::string location_str(location_template_str);
            location_str.replace(location_str_pos, 1, std::to_string(loc));
            vertex_shader.replace(pos, eol - pos, location_str);

            Attribute::Binding::Enum binding;
            if (Attribute::Binding::TryParse(binding_type, binding)) {
                bindings.Set(binding, loc);
            } else {
                log::core::Error("Can't parse {}", binding_type);
            }

            pos = vertex_shader.find(token, pos);
            loc++;
        }
    }
}

}

}