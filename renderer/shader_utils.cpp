#include "shader_utils.h"

namespace sprint {

namespace gfx {

void PreProcess(const std::string &source, std::string (&shaderSources)[ShaderType::Count]) {
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
            shaderSources[type] = (pos == std::string::npos) ?
                                  source.substr(nextLinePos) :
                                  source.substr(nextLinePos, pos - nextLinePos);
        }
    }
}

}

}