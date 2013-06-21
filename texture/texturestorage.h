#ifndef TEXTURESTORAGE_H
#define TEXTURESTORAGE_H

#include <QObject>
//#include <GL/glew.h>
#include <QImage>

class TextureStorage : public QObject
{
    Q_OBJECT

public:
    enum TexType {TEXTURE_1D = 0, TEXTURE_2D};

    /*           TEXTURE_3D, TEXTURE_1D_ARRAY, TEXTURE_2D_ARRAY,
                 TEXTURE_RECTANGLE, TEXTURE_CUBE_MAP, TEXTURE_CUBE_MAP_ARRAY, TEXTURE_BUFFER,
                 TEXTURE_2D_MULTISAMPLE, TEXTURE_2D_MULTISAMPLE_ARRAY};
    */

    enum TexFilter {NEAREST = 0, LINEAR, NEAREST_MIPMAP_NEAREST, LINEAR_MIPMAP_NEAREST,
                    NEAREST_MIPMAP_LINEAR, LINEAR_MIPMAP_LINEAR};

    enum TexWrap {CLAMP_TO_EDGE = 0, CLAMP_TO_BORDER, MIRRORED_REPEAT, REPEAT};

    TextureStorage(QObject* parent = NULL);
    TextureStorage(QString name, QString path, TexType type, QObject* parent = NULL);
    //TextureStorage(QString name, QString path, TexType type, QImage tex);


    QString getPath() const;
    QString getName() const;
    //QImage getImage() const;
    /**
     * @brief getTexId Return unique texture id.
     * @return Unique texture id.
     */
    uint getTexId() const {return texId;}
    QImage getImage() const;

    void setPath(QString path);
    inline void setName(QString name) {this->name = name; emit nameChanged(name);}
    inline void setTexId(uint texId) {this->texId = texId;}

    void setBasePath(QString path);
    bool convertPathToAbsolute(QString path);

    static QStringList getGLTypeString();
    static QStringList getGLFilterString();
    static QStringList getGLWrapString();


    void setType(TexType type);
    void setMinFilter(TexFilter filter);
    void setMagFilter(TexFilter filter);
    void setSWrap(TexWrap wrap);
    void setRWrap(TexWrap wrap);
    void setMipmap(bool generate);
    void setMipmapLevel(unsigned int level);


    TexType getType() const;
    TexFilter getMinFilter() const;
    TexFilter getMagFilter() const;
    TexWrap getSWrap() const;
    TexWrap getRWrap() const;
    bool isMipMap() const;
    unsigned int getMipMapLevel() const;

    const TextureStorage* copy();
    bool exists() const;

    friend QDataStream & operator<< (QDataStream& stream, const TextureStorage* tex);
    friend QDataStream & operator>> (QDataStream& stream, TextureStorage*& tex);

private:
    //QString toAbsolute(QString path) const;
    //void CreateTexture();
    void setupObject();

    static void saveTexture(QDataStream& stream, const TextureStorage& tex, QString basePath);
    static void loadTexture(QDataStream& stream, TextureStorage& tex);

signals:
    void nameChanged(QString name);

private:
    QString name;
    QString path;
    //QImage texture;
    uint texId;

    TexType type;
    TexFilter minFilter;
    TexFilter magFilter;
    TexWrap sWrap;
    TexWrap rWrap;
    bool generateMipMap;
    unsigned int mipmapLevel;
    QString basePath;
};

QDataStream & operator<< (QDataStream& stream, const TextureStorage* tex);
QDataStream & operator>> (QDataStream& stream, TextureStorage*& tex);

#endif // TEXTURESTORAGE_H
