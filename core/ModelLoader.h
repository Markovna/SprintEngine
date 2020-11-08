#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Log.h"

namespace sprint {

namespace model_loader {
    void Load(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Log::CoreError("Couldn't load {0}", importer.GetErrorString());
            return;
        }

        std::string directory = path.substr(0, path.find_last_of('/'));
//        processNode(scene->mRootNode, scene);
    }
};


}