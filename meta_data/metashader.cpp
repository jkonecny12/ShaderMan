#include "metashader.h"
#include <QDebug>

/**
 * @brief MetaShader::MetaShader Create empty object (serialize purpose)
 */
MetaShader::MetaShader(QObject *parent) : QObject(parent)
{
}

/**
 * @brief MetaShader::MetaShader Create new shader object.
 * @param relativePath Relative path from project base directory to shader file.
 * @param type Type of this shader (vertex, fragment etc.).
 */
MetaShader::MetaShader(QString relativePath, SHADERTYPE type, QObject *parent) : QObject(parent)
{
    this->file = relativePath;
    this->type = type;
}

/**
 * @brief MetaShader::MetaShader Create new shader object.
 * @param file File object representing shader file.
 * @param projectBaseDir Base directory of project where is this shader.
 * @param type Type of this shader (vertex, fragment, etc.).
 */
MetaShader::MetaShader(QFile file, QDir projectBaseDir, SHADERTYPE type, QObject *parent) : QObject(parent)
{
    if(QFileInfo(file).isRelative())
        this->file = QFileInfo(file).filePath();
    else
        this->file = projectBaseDir.relativeFilePath(QFileInfo(file).absoluteFilePath());

    this->type = type;
}

/**
 * @brief MetaShader::MetaShader Create new shader object.
 * @param file Path to a shader file.
 * @param projectBaseDir Base directory of project where is this shader.
 * @param type Type of this shader (vertex, fragment, etc.).
 */
MetaShader::MetaShader(QString file, QDir projectBaseDir, SHADERTYPE type, QObject *parent) : QObject(parent)
{
    if(QFileInfo(file).isRelative())
        this->file = QFileInfo(file).filePath();
    else
        this->file = projectBaseDir.relativeFilePath(QFileInfo(file).absoluteFilePath());

    this->type = type;
}


/**
 * @brief MetaShader::getShader Return shader file path
 * @return path to shader file
 */
QString MetaShader::getShader()
{
    return file;
}

/**
 * @brief MetaShader::getShaderName
 * @return
 */
QString MetaShader::getShaderName()
{
    return QFileInfo(file).fileName();
}

/**
 * @brief MetaShader::getType Return type of shader
 * @return Shader type ie. VERTEX, FRAGMENT
 */
MetaShader::SHADERTYPE MetaShader::getType()
{
    return type;
}

/**
 * @brief MetaShader::writeShaderObject Static class for saving MetaShader object to binary file.
 * @param stream Stream with opened file where we want to serialize shader.
 * @param shader Shader object we want to serialize.
 * @return Stream if needed.
 */
QDataStream& MetaShader::writeObject(QDataStream &stream, const MetaShader &shader)
{
    QString file = shader.file;
    //file = QDir::toNativeSeparators(file);
    file = file;

    stream << file;
    stream << (qint32)shader.type;

    qDebug() << "Saved MetaShader object " << shader.file;

    return stream;
}

/**
 * @brief MetaShader::readShaderObject Static class for loading MetaShader object to binary file.
 * @param stream Stream from where we want to deserialize shader.
 * @param shader Shader object where to load data from stream.
 * @return Stream if needed.
 */
QDataStream& MetaShader::readObject(QDataStream &stream, MetaShader &shader)
{
    QString file;
    stream >> file;
    //shader.file = QDir::fromNativeSeparators(file);
    shader.file = file;

    qint32 temp;
    stream >> temp;

    shader.type = static_cast<MetaShader::SHADERTYPE>(temp);

    qDebug() << "Loaded MetaShader object " << shader.file;

    return stream;
}


/** NON MEMBER of CLASS **/

/**
 * @brief operator << Operator used for serialization
 * @param stream Where we want to serialize
 * @param shader Shader for serialization
 * @return Stream for serializing more shaders in a row
 */
QDataStream & operator<< (QDataStream& stream, const MetaShader& shader)
{
    MetaShader::writeObject(stream, shader);

    return stream;
}

/**
 * @brief operator >> Operator used for deserialization
 * @param stream From where we want deserialize
 * @param shader Shader for deserialization
 * @return Stream for deserializing more shaders in a row
 */
QDataStream & operator>> (QDataStream& stream, MetaShader& shader)
{
    MetaShader::readObject(stream,shader);

    return stream;
}

/**
 * @brief operator >> Operator used for deserialization when object is a pointer
 * @param stream From where we want deserialize
 * @param shader Pointer to shader for deserialization
 * @return Stream for deserializing more shaders in a row
 */
QDataStream & operator<< (QDataStream& stream, const MetaShader* shader)
{
    MetaShader::writeObject(stream,*shader);

    return stream;
}

/**
 * @brief operator >> Operator used for deserialization when object is a pointer
 * @param stream From where we want deserialize
 * @param shader Pointer to shader for deserialization
 * @return Stream for deserializing more shaders in a row
 */
QDataStream & operator>> (QDataStream& stream, MetaShader*& shader)
{
    shader = new MetaShader(); // create new object (we have only pointer)

    MetaShader::readObject(stream,*shader);

    return stream;
}
