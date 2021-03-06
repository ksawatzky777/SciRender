#include "Graphics/Material.h"

// Project includes.
#include "Core/AssetManager.h"

namespace SciRenderer
{
  Material::Material(MaterialType type)
    : type(type)
  {
    auto shaderCache = AssetManager<Shader>::getManager();
    switch (type)
    {
      case MaterialType::PBR:
      {
        this->program = shaderCache->getAsset("pbr_shader");
        this->reflect();

        std::string texHandle;
        Texture2D::createMonoColour(glm::vec4(1.0f), texHandle);
        this->attachSampler2D("albedoMap", texHandle);
        this->attachSampler2D("metallicMap", texHandle);
        this->attachSampler2D("roughnessMap", texHandle);
        Texture2D::createMonoColour(glm::vec4(0.5f, 0.5f, 1.0f, 1.0f), texHandle);
        this->attachSampler2D("normalMap", texHandle);
        Texture2D::createMonoColour(glm::vec4(1.0f), texHandle);
        this->attachSampler2D("aOcclusionMap", texHandle);

        this->getVec3("uAlbedo") = glm::vec3(1.0f);
        this->getFloat("uMetallic") = 0.0f;
        this->getFloat("uRoughness") = 0.5f;
        this->getFloat("uAO") = 1.0f;
        break;
      }
      case MaterialType::Specular:
      {
        this->program = shaderCache->getAsset("specular_shader");
        break;
      }
      case MaterialType::GeometryPass:
      {
        this->program = shaderCache->getAsset("geometry_pass_shader");
        this->reflect();
        break;
      }
      default:
      {
        this->program = nullptr;
        break;
      }
    }
  }

  Material::~Material()
  { }

  void
  Material::reflect()
  {
    auto& shaderUniforms = this->program->getUniforms();
    for (auto& pair : shaderUniforms)
    {
      switch (pair.second)
      {
        case UniformType::Float: this->floats.push_back({ pair.first, 1.0f }); break;
        case UniformType::Vec2: this->vec2s.push_back({ pair.first, glm::vec2(1.0f) }); break;
        case UniformType::Vec3: this->vec3s.push_back({ pair.first, glm::vec3(1.0f) }); break;
        case UniformType::Vec4: this->vec4s.push_back({ pair.first, glm::vec4(1.0f) }); break;
        case UniformType::Mat3: this->mat3s.push_back({ pair.first, glm::mat3(1.0f) }); break;
        case UniformType::Mat4: this->mat4s.push_back({ pair.first, glm::mat4(1.0f) }); break;
        case UniformType::Sampler1D: this->sampler1Ds.push_back({ pair.first, "None" }); break;
        case UniformType::Sampler2D: this->sampler2Ds.push_back({ pair.first, "None" }); break;
        case UniformType::Sampler3D: this->sampler3Ds.push_back({ pair.first, "None" }); break;
        case UniformType::SamplerCube: this->samplerCubes.push_back({ pair.first, "None" }); break;
      }
    }
  }

  void
  Material::configure()
  {
    auto textureCache = AssetManager<Texture2D>::getManager();
    unsigned int samplerCount = 0;

    switch (this->type)
    {
      case MaterialType::PBR:
      {
        // Bind the PBR maps first.
        this->program->addUniformSampler("irradianceMap", 0);
      	this->program->addUniformSampler("reflectanceMap", 1);
      	this->program->addUniformSampler("brdfLookUp", 2);

        // Increase the sampler count to compensate.
        samplerCount += 3;
        break;
      }
      case MaterialType::Specular:
      {
        break;
      }
      case MaterialType::Unknown:
        break;
    }

    // Loop over 2D textures and assign them.
    for (auto& pair : this->sampler2Ds)
    {
      Texture2D* sampler = textureCache->getAsset(pair.second);

      this->program->addUniformSampler(pair.first.c_str(), samplerCount);
      if (sampler != nullptr)
        sampler->bind(samplerCount);

      samplerCount++;
    }

    // TODO: Do other sampler types (1D textures, 3D textures, cubemaps).
    for (auto& pair : this->floats)
      this->program->addUniformFloat(pair.first.c_str(), pair.second);
    for (auto& pair : this->vec2s)
      this->program->addUniformVector(pair.first.c_str(), pair.second);
    for (auto& pair : this->vec3s)
      this->program->addUniformVector(pair.first.c_str(), pair.second);
    for (auto& pair : this->vec4s)
      this->program->addUniformVector(pair.first.c_str(), pair.second);
    for (auto& pair : this->mat3s)
      this->program->addUniformMatrix(pair.first.c_str(), pair.second, GL_FALSE);
    for (auto& pair : this->mat4s)
      this->program->addUniformMatrix(pair.first.c_str(), pair.second, GL_FALSE);

    this->program->bind();
  }

  void
  Material::configure(Shader* overrideProgram)
  {
    auto textureCache = AssetManager<Texture2D>::getManager();
    unsigned int samplerCount = 0;

    switch (this->type)
    {
      case MaterialType::PBR:
      {
        // Bind the PBR maps first.
        overrideProgram->addUniformSampler("irradianceMap", 0);
      	overrideProgram->addUniformSampler("reflectanceMap", 1);
      	overrideProgram->addUniformSampler("brdfLookUp", 2);

        // Increase the sampler count to compensate.
        samplerCount += 3;
        break;
      }
      case MaterialType::Specular:
      {
        break;
      }
      case MaterialType::Unknown:
        break;
    }

    // Loop over 2D textures and assign them.
    for (auto& pair : this->sampler2Ds)
    {
      Texture2D* sampler = textureCache->getAsset(pair.second);

      overrideProgram->addUniformSampler(pair.first.c_str(), samplerCount);
      if (sampler != nullptr)
        sampler->bind(samplerCount);

      samplerCount++;
    }

    // TODO: Do other sampler types (1D textures, 3D textures, cubemaps).
    for (auto& pair : this->floats)
      overrideProgram->addUniformFloat(pair.first.c_str(), pair.second);
    for (auto& pair : this->vec2s)
      overrideProgram->addUniformVector(pair.first.c_str(), pair.second);
    for (auto& pair : this->vec3s)
      overrideProgram->addUniformVector(pair.first.c_str(), pair.second);
    for (auto& pair : this->vec4s)
      overrideProgram->addUniformVector(pair.first.c_str(), pair.second);
    for (auto& pair : this->mat3s)
      overrideProgram->addUniformMatrix(pair.first.c_str(), pair.second, GL_FALSE);
    for (auto& pair : this->mat4s)
      overrideProgram->addUniformMatrix(pair.first.c_str(), pair.second, GL_FALSE);

    overrideProgram->bind();
  }

  // Search for and attach textures to a sampler.
  bool
  Material::hasSampler1D(const std::string &samplerName)
  {
    return Utilities::pairSearch<std::string, std::string>(this->sampler1Ds, samplerName);
  }
  void
  Material::attachSampler1D(const std::string &samplerName, const SciRenderer::AssetHandle &handle)
  {
    if (!this->hasSampler1D(samplerName))
      this->sampler1Ds.push_back(std::pair(samplerName, handle));
    else
    {
      auto loc = Utilities::pairGet<std::string, std::string>(this->sampler1Ds, samplerName);

      this->sampler1Ds.erase(loc);
      this->sampler1Ds.push_back(std::pair(samplerName, handle));
    }
  }

  bool
  Material::hasSampler2D(const std::string &samplerName)
  {
    return Utilities::pairSearch<std::string, std::string>(this->sampler2Ds, samplerName);
  }
  void
  Material::attachSampler2D(const std::string &samplerName, const SciRenderer::AssetHandle &handle)
  {
    if (!this->hasSampler2D(samplerName))
      this->sampler2Ds.push_back(std::pair(samplerName, handle));
    else
    {
      auto loc = Utilities::pairGet<std::string, std::string>(this->sampler2Ds, samplerName);

      this->sampler2Ds.erase(loc);
      this->sampler2Ds.push_back(std::pair(samplerName, handle));
    }
  }

  bool
  Material::hasSampler3D(const std::string &samplerName)
  {
    return Utilities::pairSearch<std::string, std::string>(this->sampler3Ds, samplerName);
  }
  void
  Material::attachSampler3D(const std::string &samplerName, const SciRenderer::AssetHandle &handle)
  {
    if (!this->hasSampler3D(samplerName))
      this->sampler3Ds.push_back(std::pair(samplerName, handle));
    else
    {
      auto loc = Utilities::pairGet<std::string, std::string>(this->sampler3Ds, samplerName);

      this->sampler3Ds.erase(loc);
      this->sampler3Ds.push_back(std::pair(samplerName, handle));
    }
  }

  bool
  Material::hasSamplerCubemap(const std::string &samplerName)
  {
    return Utilities::pairSearch<std::string, std::string>(this->samplerCubes, samplerName);
  }
  void
  Material::attachSamplerCubemap(const std::string &samplerName, const SciRenderer::AssetHandle &handle)
  {
    if (!this->hasSamplerCubemap(samplerName))
      this->samplerCubes.push_back(std::pair(samplerName, handle));
    else
    {
      auto loc = Utilities::pairGet<std::string, std::string>(this->samplerCubes, samplerName);

      this->samplerCubes.erase(loc);
      this->samplerCubes.push_back(std::pair(samplerName, handle));
    }
  }

  Texture2D*
  Material::getSampler2D(const std::string &samplerName)
  {
    auto textureCache = AssetManager<Texture2D>::getManager();

    auto loc = Utilities::pairGet<std::string, std::string>(this->sampler2Ds, samplerName);

    return textureCache->getAsset(loc->second);
  }
  SciRenderer::AssetHandle&
  Material::getSampler2DHandle(const std::string &samplerName)
  {
    auto loc = Utilities::pairGet<std::string, std::string>(this->sampler2Ds, samplerName);
    return loc->second;
  }

  // Attach a mesh-material pair.
  void
  ModelMaterial::attachMesh(const std::string &meshName, MaterialType type)
  {
    if (!Utilities::pairSearch<std::string, Material>(this->materials, meshName))
      this->materials.push_back(std::make_pair(meshName, Material(type)));
  }

  // Get a material given the mesh.
  Material*
  ModelMaterial::getMaterial(const std::string &meshName)
  {
    auto loc = Utilities::pairGet<std::string, Material>(this->materials, meshName);

    if (loc != this->materials.end())
      return &loc->second;

    return nullptr;
  }
}
