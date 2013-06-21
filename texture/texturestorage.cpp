#include "texturestorage.h"
#include <QDir>

/**
 * @brief TextureStorage::TextureStorage Create empty texture object
 * @param parent Parent of this object.
 */
TextureStorage::TextureStorage(QObject *parent) :
    QObject(parent)
{
}

/**
 * @brief TextureStorage::TextureStorage Create texture object
 * @param name Texture name
 * @param path Texture path (absolute or relative)
 * @param type Type of the texture (GL_TEXTURE, GL_TEXTURE_2D, GL_TEXTURE_3D etc).
 * @param parent Parent of this object.
 */
TextureStorage::TextureStorage(QString name, QString path, TexType type, QObject *parent) :
    QObject(parent),
    texId(0)
{
    this->name = name;
    this->path = path;
    //this->path = QDir::toNativeSeparators(path);;
    //this->path = toAbsolute(this->path);

    this->type = type;

    setupObject();
}

/**
 * @brief TextureStorage::TextureStorage Create texture object
 * @param name Texture name
 * @param type Type of the texture (GL_TEXTURE, GL_TEXTURE_2D, GL_TEXTURE_3D etc).
 * @param tex Image with texture
 */
/*
TextureStorage::TextureStorage(QString name, QString path, TexType type, QImage tex) :
    QObject(0),
    texId(0)
{
    this->name = name;
    this->path = QDir::toNativeSeparators(path);;
    //this->path = toAbsolute(this->path);

    this->type = type;
    texture = tex;

    setupObject();
}
*/

/**
 * @brief TextureStorage::getPath Get absolute path to this texture.
 * @return Relative path from project base directory.
 */
QString TextureStorage::getPath() const
{
    return path;
}

/**
 * @brief TextureStorage::getName Get name of this texture.
 * @return Return name.
 */
QString TextureStorage::getName() const
{
    return name;
}

/**
 * @brief TextureStorage::getImage Return texture image.
 * @return Texture image.
 */
QImage TextureStorage::getImage() const
{
    if(path.isEmpty())
        return QImage();

    return QImage(path);
}

/**
 * @brief TextureStorage::getImage Return texture as QImage
 * @return Texture as image
 */
/*
QImage TextureStorage::getImage() const
{
    if(!QDir().exists(toAbsolute(path)))
    {
        return QImage();
    }

    return QImage(path);
}
*/

/**
 * @brief TextureStorage::setPath Set absolute path of texture
 * @param path Absolute path to texture
 */
void TextureStorage::setPath(QString path)
{
    //path = QDir::toNativeSeparators(path);
    this->path = path;
    //this->path = toAbsolute(this->path);
}

/**
 * @brief TextureStorage::setBasePath Set project base path, it will be used when saving texture to convert path to relative path.
 * @param path Project base path.
 */
void TextureStorage::setBasePath(QString path)
{
    this->basePath = path;
}

/**
 * @brief TextureStorage::convertPathToAbsolute Convert loaded relative path from project to absolute path.
 * @param path Project base path.
 * @return True if texture file exists, false if not.
 */
bool TextureStorage::convertPathToAbsolute(QString path)
{
    //path = QDir::toNativeSeparators(path);

    QDir base(path);

    this->path = base.absoluteFilePath(this->path);

    QFile f(this->path);

    if(!f.exists())
        return false;

    return true;
}

/**
 * @brief TextureStorage::getGLTypeString Get list of strings with TexType enum as strings in the same order.
 * @return Return TexType as strings.
 */
QStringList TextureStorage::getGLTypeString()
{
    QStringList ret;
    ret << "TEXTURE_1D" << "TEXTURE_2D";
           /* << "TEXTURE_3D" << "TEXTURE_1D_ARRAY" << "TEXTURE_2D_ARRAY"
        << "TEXTURE_RECTANGLE" << "TEXTURE_CUBE_MAP" << "TEXTURE_CUBE_MAP_ARRAY" << "TEXTURE_BUFFER"
        << "TEXTURE_2D_MULTISAMPLE" << "TEXTURE_2D_MULTISAMPLE_ARRAY";
            */
    return ret;
}

/**
 * @brief TextureStorage::getGLFilterString Get list of strings with TexFilter enum as strings in the same order.
 * @return Return TexFilter as strings.
 */
QStringList TextureStorage::getGLFilterString()
{
    QStringList ret;
    ret << "NEAREST" << "LINEAR" << "NEAREST_MIPMAP_NEAREST" << "LINEAR_MIPMAP_NEAREST"
        << "NEAREST_MIPMAP_LINEAR" << "LINEAR_MIPMAP_LINEAR";

    return ret;
}

/**
 * @brief TextureStorage::getGLWrapString Get list of strings with TexWrap enum as strings in the same order.
 * @return Return TexWrap as strings
 */
QStringList TextureStorage::getGLWrapString()
{
    QStringList ret;
    ret << "CLAMP_TO_EDGE" << "CLAMP_TO_BORDER" << "MIRRORED_REPEAT" << "REPEAT";

    return ret;
}

/**
 * @brief TextureStorage::setType Set texture type for this texture.
 * @param type Texture type (TEXTURE_1D, TEXTURE_2D).
 */
void TextureStorage::setType(TextureStorage::TexType type)
{
    this->type = type;
}

/**
 * @brief TextureStorage::setMinFilter Set texture minifying filter for this texture.
 * @param filter Filter function we want to set.
 */
void TextureStorage::setMinFilter(TextureStorage::TexFilter filter)
{
    this->minFilter = filter;
}

/**
 * @brief TextureStorage::setMagFilter Set texture magnification filter for this texture.
 * @param filter Filter function we want to set.
 */
void TextureStorage::setMagFilter(TextureStorage::TexFilter filter)
{
    if(filter == NEAREST || filter == LINEAR)
    {
        magFilter = filter;
    }
}

/**
 * @brief TextureStorage::setSWrap Set the wrap parameter to this texture S axis.
 * @param wrap Wrap parameter we want set.
 */
void TextureStorage::setSWrap(TextureStorage::TexWrap wrap)
{
    this->sWrap = wrap;
}

/**
 * @brief TextureStorage::setRWrap Set the wrap parameter to this texture R axis.
 * @param wrap Wrap parameter we  want set.
 */
void TextureStorage::setRWrap(TextureStorage::TexWrap wrap)
{
    this->rWrap = wrap;
}

/**
 * @brief TextureStorage::setMipmap Turn on or off mipmap generation.
 * @param generate If true mipmap will be generated.
 */
void TextureStorage::setMipmap(bool generate)
{
    this->generateMipMap = generate;
}

/**
 * @brief TextureStorage::setMipmapLevel Set generated mipmap level.
 * @param level Level of generated mipmap.
 */
void TextureStorage::setMipmapLevel(unsigned int level)
{
    mipmapLevel = level;
}

/**
 * @brief TextureStorage::getType Get OpenGL type of this texture.
 * @return Return texture type.
 */
TextureStorage::TexType TextureStorage::getType() const
{
    return type;
}

/**
 * @brief TextureStorage::getMinFilter Get minifying filter from this texture.
 * @return Return minifying filter.
 */
TextureStorage::TexFilter TextureStorage::getMinFilter() const
{
    return minFilter;
}

/**
 * @brief TextureStorage::getMagFilter Get magnification filter from this texture.
 * @return Return magnification filter.
 */
TextureStorage::TexFilter TextureStorage::getMagFilter() const
{
    return magFilter;
}

/**
 * @brief TextureStorage::getSWrap Get OpenGL S wrap parameter from this texture.
 * @return Return S wrap parameter.
 */
TextureStorage::TexWrap TextureStorage::getSWrap() const
{
    return sWrap;
}

/**
 * @brief TextureStorage::getRWrap Get OpenGL R wrap parameter from this texture.
 * @return Return R wrap parameter.
 */
TextureStorage::TexWrap TextureStorage::getRWrap() const
{
    return rWrap;
}

/**
 * @brief TextureStorage::isMipMap Are we using mipmap generation for this texture.
 * @return Return true if user want generate mipmap, false otherwise.
 */
bool TextureStorage::isMipMap() const
{
    return generateMipMap;
}

/**
 * @brief TextureStorage::getMipMapLevel Return level of generated mipmap.
 * @return Return mipmap generated level.
 */
unsigned int TextureStorage::getMipMapLevel() const
{
    return mipmapLevel;
}

/**
 * @brief TextureStorage::copy Create and return copy of this object.
 * @return Return copy of this object.
 */
const TextureStorage *TextureStorage::copy()
{
    TextureStorage* tex = new TextureStorage(this->getName(), this->getPath(), this->getType());

    tex->generateMipMap = this->generateMipMap;
    tex->minFilter = this->minFilter;
    tex->magFilter = this->magFilter;
    tex->rWrap = this->rWrap;
    tex->sWrap = this->sWrap;

    return tex;
}

/**
 * @brief TextureStorage::exists Test if texture have correct path to a file.
 * @return Return tru if texture file exist, otherwise false.
 */
bool TextureStorage::exists() const
{
    return QFileInfo(this->path).exists();
}

/**
 * @brief TextureStorage::toAbsolute Convert relative path to absolute path, if path is already absolute return this path
 * @param path Relative or absolute path
 * @return Absolute path
 */
/*
QString TextureStorage::toAbsolute(QString path) const
{
    if(QDir(path).isRelative())
        return QDir(path).absolutePath();
    else
        return path;
}
*/

/**
 * @brief TextureStorage::setupObject Set default values to this object.
 */
void TextureStorage::setupObject()
{
    minFilter = LINEAR;
    magFilter = LINEAR;

    sWrap = REPEAT;
    rWrap = REPEAT;

    generateMipMap = false;
    mipmapLevel = 0;
}

/**
 * @brief TextureStorage::saveTexture Save texture to stream.
 * @param stream Where to save this texture.
 * @param tex What to save.
 */
void TextureStorage::saveTexture(QDataStream &stream, const TextureStorage &tex, QString basePath)
{
    stream << tex.name;

    QString relPath;
    if(!basePath.isEmpty())
    {
        QDir d(basePath);

        relPath = d.relativeFilePath(tex.path);
        stream << relPath;
    }
    else
    {
        stream << tex.path;
    }

    //stream << tex.path;
    stream << static_cast<quint32>(tex.texId);

    stream << static_cast<qint32>(tex.type);
    stream << static_cast<qint32>(tex.minFilter);
    stream << static_cast<qint32>(tex.magFilter);
    stream << static_cast<qint32>(tex.sWrap);
    stream << static_cast<qint32>(tex.rWrap);
    stream << tex.generateMipMap;
    stream << tex.mipmapLevel;
}

/**
 * @brief TextureStorage::loadTexture Load texture from stream.
 * @param stream From where we load texture.
 * @param tex Load this texture.
 */
void TextureStorage::loadTexture(QDataStream &stream, TextureStorage &tex)
{
    stream >> tex.name;
    stream >> tex.path;
    quint32 v;
    stream >> v;
    tex.texId = v;

    qint32 vi;
    stream >> vi;
    tex.type = static_cast<TexType>(vi);

    stream >> vi;
    tex.minFilter = static_cast<TexFilter>(vi);

    stream >> vi;
    tex.magFilter = static_cast<TexFilter>(vi);

    stream >> vi;
    tex.sWrap = static_cast<TexWrap>(vi);

    stream >> vi;
    tex.rWrap = static_cast<TexWrap>(vi);

    stream >> tex.generateMipMap;
    stream >> tex.mipmapLevel;
}

/**
 * @brief TextureStorage::CreateTexture Create texture and aply all settings
 */
/*
void TextureStorage::CreateTexture()
{
    if(texId == 0)
        return;

    //bindTexture();
}
*/

/**
 * @brief operator << Save texture to stream.
 * @param stream Stream where to serialize.
 * @param tex What we want to serialize.
 * @return Return stream for another use.
 */
QDataStream &operator <<(QDataStream &stream, const TextureStorage *tex)
{
    tex->saveTexture(stream, *tex, tex->basePath);

    return stream;
}

/**
 * @brief operator >> Load texture to stream.
 * @param stream Stream from where we load.
 * @param tex Where to load texture. We also create new texture.
 * @return Return stream for another use.
 */
QDataStream &operator >>(QDataStream &stream, TextureStorage *&tex)
{
    tex = new TextureStorage();

    tex->loadTexture(stream, *tex);

    return stream;
}
