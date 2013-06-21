#include "gltexture.h"
#include <QGLWidget>

/**
 * @brief GLTexture::GLTexture Create OpenGL texture and communicate with it through this object.
 * @param texture Texture storage with information needed for texture creating.
 */
GLTexture::GLTexture(const TextureStorage *texture) :
    texture(texture)
{
    convertEnums();
}

/**
 * @brief GLTexture::~GLTexture Destroy texture storage and OpenGL texture too.
 */
GLTexture::~GLTexture()
{
    glDeleteTextures(1, &texId);
    delete texture;
}

/**
 * @brief GLTexture::createOGLTexture Create OpenGL texture.
 * @return True if texture is created good, false otherwise.
 */
bool GLTexture::createOGLTexture()
{
    glGenTextures(1,&texId);

    glBindTexture(target, texId);

    /*
    int mipmapLevel = 0;

    if(texture->isMipMap())
        mipmapLevel = texture->getMipMapLevel();

    */

    QImage image = QGLWidget::convertToGLFormat(texture->getImage());

    switch(target)
    {
    case GL_TEXTURE_2D:
        glTexImage2D(target, 0, GL_RGBA, image.width(), image.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        break;
    case GL_TEXTURE_1D:
        glTexImage1D(target, 0, GL_RGBA, image.width(), 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, image.bits());
        break;
    default:
        glDeleteTextures(1, &texId);
        return false;
        break;
    }

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, clampS);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, clampR);

    if(texture->isMipMap())
        glGenerateMipmap(target);

    return true;
}

/**
 * @brief GLTexture::useTexture Activate texture on given texture unit.
 * @param uniformLocation Location of texture sampler in shader.
 * @param texUnit OpenGL texture unit, where we want attach this texture.
 * @return Return true if texture unit is in implementation dependand bounds (minimum 80), false otherwise.
 */
bool GLTexture::useTexture(int uniformLocation, int texUnit)
{
    if(texUnit >= 0 && texUnit < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)
    {
        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(target, texId);
        glUniform1i(uniformLocation, texUnit);

        glActiveTexture(GL_TEXTURE0); // need to reset - who know why??
        return true;
    }

    return false;
}

/**
 * @brief GLTexture::convertEnums Convert all TextureStorage enums to OpenGL enums.
 */
void GLTexture::convertEnums()
{
    target = convertTypeToGL(texture->getType());
    minFilter = convertFilterToGL(texture->getMinFilter());
    magFilter = convertFilterToGL(texture->getMagFilter());
    clampS = convertClampToGL(texture->getSWrap());
    clampR = convertClampToGL(texture->getRWrap());
}

/**
 * @brief GLTexture::convertToGL Convert type enums to OpenGL enums.
 * @param type TextureStorage type enum.
 * @return OpenGL target enum.
 */
GLenum GLTexture::convertTypeToGL(const TextureStorage::TexType type)
{
    switch(type)
    {
    case TextureStorage::TEXTURE_1D:
        return GL_TEXTURE_1D;
        break;
    case TextureStorage::TEXTURE_2D:
        return GL_TEXTURE_2D;
        break;
    }

    return 0;
}

/**
 * @brief GLTexture::convertFilterToGL Convert filter enums to OpenGL enums.
 * @param filter TextureStorage filter enum.
 * @return OpenGL enum.
 */
GLenum GLTexture::convertFilterToGL(const TextureStorage::TexFilter filter)
{
    switch(filter)
    {
    case TextureStorage::NEAREST:
        return GL_NEAREST;
        break;
    case TextureStorage::LINEAR:
        return GL_LINEAR;
        break;
    case TextureStorage::NEAREST_MIPMAP_NEAREST:
        return GL_NEAREST_MIPMAP_NEAREST;
        break;
    case TextureStorage::LINEAR_MIPMAP_NEAREST:
        return GL_LINEAR_MIPMAP_NEAREST;
        break;
    case TextureStorage::NEAREST_MIPMAP_LINEAR:
        return GL_NEAREST_MIPMAP_LINEAR;
        break;
    case TextureStorage::LINEAR_MIPMAP_LINEAR:
        return GL_LINEAR_MIPMAP_LINEAR;
        break;
    }

    return 0;
}

/**
 * @brief GLTexture::convertClampToGL Convert wrap enums to OpenGL enums.
 * @param wrap TextureStorage wrap enum.
 * @return OpenGL enum.
 */
GLenum GLTexture::convertClampToGL(const TextureStorage::TexWrap wrap)
{
    switch(wrap)
    {
    case TextureStorage::CLAMP_TO_EDGE:
        return GL_CLAMP_TO_EDGE;
        break;
    case TextureStorage::CLAMP_TO_BORDER:
        return GL_CLAMP_TO_BORDER;
        break;
    case TextureStorage::MIRRORED_REPEAT:
        return GL_MIRRORED_REPEAT;
        break;
    case TextureStorage::REPEAT:
        return GL_REPEAT;
        break;
    }

    return 0;
}
