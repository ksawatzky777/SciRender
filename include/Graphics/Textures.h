#pragma once

// Macro include file.
#include "SciRenderPCH.h"

// Project includes.
#include "Core/ApplicationBase.h"

// STL includes.
#include <mutex>

namespace SciRenderer
{
  // Parameters for textures.
  enum class TextureInternalFormats
  {
    Depth = GL_DEPTH_COMPONENT, DepthStencil = GL_DEPTH_STENCIL,
    Depth24Stencil8 = GL_DEPTH24_STENCIL8, Depth32f = GL_DEPTH_COMPONENT32F,
    Red = GL_RED, RG = GL_RG, RGB = GL_RGB, RGBA = GL_RGBA, R16f = GL_R16F,
    RG16f = GL_RG16F, RGB16f = GL_RGB16F, RGBA16f = GL_RGBA16F, R32f = GL_R32F,
    RG32f = GL_RG32F, RGB32f = GL_RGB32F, RGBA32f = GL_RGBA32F
  };
  enum class TextureFormats
  {
    Depth = GL_DEPTH_COMPONENT, DepthStencil = GL_DEPTH_STENCIL, Red = GL_RED,
    RG = GL_RG, RGB = GL_RGB, RGBA = GL_RGBA
  };
  enum class TextureDataType
  {
    Bytes = GL_UNSIGNED_BYTE, Floats = GL_FLOAT,
    UInt24UInt8 = GL_UNSIGNED_INT_24_8
  };
  enum class TextureWrapParams
  {
    ClampEdges = GL_CLAMP_TO_EDGE, ClampBorder = GL_CLAMP_TO_BORDER,
    MirroredRepeat =  GL_MIRRORED_REPEAT, Repeat = GL_REPEAT,
    MirrorClampEdges =  GL_MIRROR_CLAMP_TO_EDGE
  };
  enum class TextureMinFilterParams
  {
    Nearest = GL_NEAREST, Linear = GL_LINEAR,
    NearestMipMapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipMapNearest = GL_LINEAR_MIPMAP_NEAREST,
    LinearMipMapLinear = GL_LINEAR_MIPMAP_LINEAR
  };
  enum class TextureMaxFilterParams
  {
    Nearest = GL_NEAREST, Linear = GL_LINEAR
  };
  enum class TextureType
  {
    Tex2 = GL_TEXTURE_2D,
    PosX = GL_TEXTURE_CUBE_MAP_POSITIVE_X, NegX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    PosY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y, NegY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    PosZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z, NegZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
  };

  // Parameters for loading textures.
  struct Texture2DParams
  {
    TextureWrapParams      sWrap;
    TextureWrapParams      tWrap;
    TextureMinFilterParams minFilter;
    TextureMaxFilterParams maxFilter;

    Texture2DParams()
      : sWrap(TextureWrapParams::Repeat)
      , tWrap(TextureWrapParams::Repeat)
      , minFilter(TextureMinFilterParams::Linear)
      , maxFilter(TextureMaxFilterParams::Linear)
    { };
  };

  struct TextureCubeMapParams
  {
    TextureWrapParams      sWrap;
    TextureWrapParams      tWrap;
    TextureWrapParams      rWrap;
    TextureMinFilterParams minFilter;
    TextureMaxFilterParams maxFilter;

    TextureCubeMapParams()
      : sWrap(TextureWrapParams::ClampEdges)
      , tWrap(TextureWrapParams::ClampEdges)
      , rWrap(TextureWrapParams::ClampEdges)
      , minFilter(TextureMinFilterParams::Linear)
      , maxFilter(TextureMaxFilterParams::Linear)
    { };
  };

  struct ImageData2D
  {
    int width;
    int height;
    int n;

    void* data;

    bool isHDR;
    Texture2DParams params;
    std::string name;
    std::string filepath;
  };

  //----------------------------------------------------------------------------
  // 2D textures.
  //----------------------------------------------------------------------------
  class Texture2D
  {
  public:
    // Members to facilitate asynchronous image loading.
    static std::queue<ImageData2D> asyncTexQueue;
    static std::mutex asyncTexMutex;

    static void bulkGenerateTextures();
    static void loadImageAsync(const std::string &filepath,
                               const Texture2DParams &params = Texture2DParams());

    // Other members to load and generate textures.
    static Texture2D* createMonoColour(const glm::vec4 &colour, std::string &outName,
                                       const Texture2DParams &params = Texture2DParams(),
                                       bool cache = true);
    static Texture2D* createMonoColour(const glm::vec4 &colour, const Texture2DParams &params = Texture2DParams(),
                                       bool cache = true);

    // This loads an image and generates the texture all at once, does so on the
    // main thread due to OpenGL thread safety.
    static Texture2D* loadTexture2D(const std::string &filepath, const Texture2DParams &params
                                    = Texture2DParams(), bool cache = true);

    // The actual 2D texture class.
    Texture2D();
    Texture2D(const GLuint &width, const GLuint &height, const GLuint &n,
              const Texture2DParams &params = Texture2DParams());
    ~Texture2D();

    int width;
    int height;
    int n;
    Texture2DParams params;

    // Bind/unbind the texture.
    void bind();
    void bind(GLuint bindPoint);
    void unbind();
    void unbind(GLuint bindPoint);

    GLuint& getID() { return this->textureID; }
    std::string& getFilepath() { return this->filepath; }
  private:
    GLuint textureID;

    std::string filepath;
  };

  //----------------------------------------------------------------------------
  // Cubemap textures.
  //----------------------------------------------------------------------------
  class CubeMap
  {
  public:
    CubeMap();
    CubeMap(const GLuint &width, const GLuint &height, const GLuint &n,
            const TextureCubeMapParams &params = TextureCubeMapParams());
    ~CubeMap();

    int width[6];
    int height[6];
    int n[6];
    TextureCubeMapParams params;

    // Bind/unbind the texture.
    void bind();
    void bind(GLuint bindPoint);
    void unbind();
    void unbind(GLuint bindPoint);

    GLuint& getID() { return this->textureID; }
    std::string& getFilepath() { return this->filepath; }
  private:
    GLuint textureID;

    std::string filepath;
  };
}
