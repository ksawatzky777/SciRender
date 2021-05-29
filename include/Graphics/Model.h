#pragma once

// Macro include file.
#include "SciRenderPCH.h"

// Project includes.
#include "Core/ApplicationBase.h"
#include "Graphics/Meshes.h"
#include "Graphics/Textures.h"

// Assimp so more than just obj files can be loaded.
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace SciRenderer
{
  // Model class
  class Model
  {
  public:
    Model();
    ~Model();

    void loadModel(const std::string &filepath);

    // Get the submeshes for the model.
    std::vector<Shared<Mesh>>& getSubmeshes() { return this->subMeshes; }
    std::string getFilepath() { return this->filepath; }
  protected:
    bool loaded;
    std::vector<Shared<Mesh>> subMeshes;
    std::vector<Shared<Texture2D>> textures;

    std::string filepath;
    std::string name;

  private:
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);

    friend class Mesh;
  };
}