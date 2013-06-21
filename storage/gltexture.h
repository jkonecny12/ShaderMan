#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "texture/texturestorage.h"
#define GLEW_STATIC
#include <GL/glew.h>

class GLTexture
{
public:
    explicit GLTexture(const TextureStorage *texture);
    ~GLTexture();

    bool createOGLTexture();
    bool useTexture(int uniformLocation, int texUnit = 0);

private:
    void convertEnums();
    GLenum convertTypeToGL(const TextureStorage::TexType type);
    GLenum convertFilterToGL(const TextureStorage::TexFilter filter);
    GLenum convertClampToGL(const TextureStorage::TexWrap wrap);

private:
    const TextureStorage* texture;
    GLuint texId;
    GLenum target;
    GLenum minFilter;
    GLenum magFilter;
    GLenum clampS;
    GLenum clampR;

};

#endif // GLTEXTURE_H
