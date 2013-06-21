#include "metashaderprog.h"
#include <QDebug>
#include <QFileInfo>

/**
 * @brief MetaShaderProg::MetaShaderProg Create new empty shader program object.
 */
MetaShaderProg::MetaShaderProg(QObject *parent) : QObject(parent)
{
}

/**
 * @brief MetaShaderProg::MetaShaderProg Create new shader program object.
 * @param name Name of this shader program. Must be unique.
 * @param projectBase Base directory of project which contains this object.
 */
MetaShaderProg::MetaShaderProg(QString name, QDir projectBase, QObject *parent) : QObject(parent)
{
    programName = name;
    baseDir = projectBase;
}

/**
 * @brief MetaShaderProg::MetaShaderProg Create new shader program object.
 * @param vertex Path to a vertex shader.
 * @param fragment Path to a fragment shader.
 * @param name Name of this shader program. Must be unique.
 * @param projectBase Base directory of project which contains this object.
 */
MetaShaderProg::MetaShaderProg(QString vertex, QString fragment, QString name, QDir projectBase, QObject *parent) :
    QObject(parent)
{
    this->vertex = vertex;
    this->fragment = fragment;
    programName = name;
    baseDir = projectBase;
}

/**
 * @brief MetaShaderProg::copy Copy this instance of object and return this copy.
 * @return Return constant copy of this object.
 */
const MetaShaderProg *MetaShaderProg::copy()
{
    MetaShaderProg *prog = new MetaShaderProg();

    prog->programName = this->programName;
    prog->vertex = this->vertex;
    prog->fragment = this->fragment;
    prog->verticesAttach = this->verticesAttach;
    prog->normalsAttach = this->normalsAttach;
    prog->colorsAttach = this->colorsAttach;
    prog->texCoordsAttach = this->texCoordsAttach;
    prog->texturesAttach = this->texturesAttach;
    prog->baseDir = this->baseDir;

    return prog;
}

/**
 * @brief MetaShaderProg::setProgramName Set name of this shader program and emit signal nameChanged.
 * @param name New shader program name. New name must be unique in project.
 */
void MetaShaderProg::setProgramName(QString name)
{
    QString old = this->programName;
    this->programName = name;

    emit nameChanged(old, name);
}

/**
 * @brief MetaShaderProg::setProjectBaseDir Set project base directory,
 *this is needed for retrieving absolute path for shaders
 * @param base Project base directory
 */
void MetaShaderProg::setProjectBaseDir(QDir base)
{
    baseDir = base;
}

/**
 * @brief MetaShaderProg::setVertexShader Set vertex shader to this shader program
 * @param shader Vertex shader
 */
void MetaShaderProg::setVertexShader(QString shader)
{
    this->vertex = shader;

    qDebug() << "Setting vertex shader " << shader << "to program " << programName;
}

/**
 * @brief MetaShaderProg::setFragmentShader Set fragment shader to this shader program
 * @param shader Fragment shader
 */
void MetaShaderProg::setFragmentShader(QString shader)
{
    this->fragment = shader;

    qDebug() << "Setting fragment shader " << shader << "to program " << programName;
}

/**
 * @brief MetaShaderProg::getVertexShader Return vertex shader linked to this shader program
 * @return Relative path to a vertex shader
 */
QString MetaShaderProg::getVertexShader() const
{
    return vertex;
}

/**
 * @brief MetaShaderProg::getFragmentShader Return fragment shader linked to this shader program
 * @return Relative path to a fragment shader
 */
QString MetaShaderProg::getFragmentShader() const
{
    return fragment;
}

/**
 * @brief MetaShaderProg::getVertexAbsolutePath Return vertex shader linked to this shader program
 * @return Absolute path to a vertex shader
 */
QString MetaShaderProg::getVertexAbsolutePath() const
{
    return QFileInfo(baseDir,vertex).absoluteFilePath();
}

/**
 * @brief MetaShaderProg::getFragmentAbsolutePath Return fragment shader linked to this shader program
 * @return Absolute path to a fragment shader
 */
QString MetaShaderProg::getFragmentAbsolutePath() const
{
    return QFileInfo(baseDir,fragment).absoluteFilePath();
}

/**
 * @brief MetaShaderProg::getShaders Return attached shaders (all types)
 * @return Shader paths
 */
QStringList MetaShaderProg::getShaders() const
{
    QStringList list;

    if(!vertex.isEmpty())
        list << vertex;
    if(!fragment.isEmpty())
        list << fragment;

    return list;
}

/**
 * @brief MetaShaderProg::getColors Get attachment point names for colors.
 * @return Return list with names of attachment points
 */
QStringList MetaShaderProg::getColorNames() const
{
    return QStringList(colorsAttach.values());
}

/**
 * @brief MetaShaderProg::getTexCoords Get attachment point names for texture coordinates.
 * @return Return list with names of attachment points.
 */
QStringList MetaShaderProg::getTexCoordNames() const
{
    return QStringList(texCoordsAttach.values());
}

/**
 * @brief MetaShaderProg::getTextureAttachment Get all attachment points set to this shader program.
 * @param textureName Name of the texture we want attachment to a points.
 * @return Return attachment points.
 */
/*
QStringList MetaShaderProg::getTextureAttachment(const QString textureName)
{
    return QStringList(texturesAttach.keys(textureName)->toList());
}
*/

/**
 * @brief MetaShaderProg::getTextureNames Get texture names attached to this shader program.
 * @return Return names of textures attached.
 */
QStringList MetaShaderProg::getTextureNames() const
{
    return QStringList(texturesAttach.values());
}

/**
 * @brief MetaShaderProg::getTexturePoints Get all points attached used in this shader program.
 * @return Return all attached points.
 */
QStringList MetaShaderProg::getTexturePoints() const
{
    return QStringList(texturesAttach.keys());
}

/**
 * @brief MetaShaderProg::getTexture Get texture name attached to point.
 * @param pointName Texture attachment point name.
 * @return Return texture name.
 */
QString MetaShaderProg::getTexture(QString pointName) const
{
    return texturesAttach.value(pointName);
}

/**
 * @brief MetaShaderProg::getColor Get name of the color attachment from position.
 * @param position Position in color buffer list.
 * @return Return name of the color attachment.
 */
QString MetaShaderProg::getColor(uint position) const
{
    return colorsAttach.value(position);
}

/**
 * @brief MetaShaderProg::getColor Get color position from given color name.
 * @param name Name of the color attachment.
 * @return Return position of this color attachment in color buffer.
 */
uint MetaShaderProg::getColor(QString name) const
{
    return colorsAttach.key(name);
}

/**
 * @brief MetaShaderProg::getTexCoord Get name of the texture coordinate attachment from position.
 * @param position Position in texture coordinate buffer list.
 * @return Return name of the texture coordinate attachment.
 */
QString MetaShaderProg::getTexCoord(uint position) const
{
    return texCoordsAttach.value(position);
}

/**
 * @brief MetaShaderProg::getTexCoord Get texture coordinate position from given name.
 * @param name Name of the texture coordinate attachment.
 * @return Return position of this texture coordinate attachment in buffer list.
 */
uint MetaShaderProg::getTexCoord(QString name) const
{
    return texCoordsAttach.key(name);
}

/**
 * @brief MetaShaderProg::getColorPositions Get all attached positions in color buffers.
 * @return Return all attached positions.
 */
QList<uint> MetaShaderProg::getColorPositions() const
{
    return colorsAttach.keys();
}

/**
 * @brief MetaShaderProg::getTexCoordPositions Get all attached positions in texture coordinate buffers.
 * @return Return all attached positions.
 */
QList<uint> MetaShaderProg::getTexCoordPositions() const
{
    return texCoordsAttach.keys();
}

/**
 * @brief MetaShaderProg::addColor Add new attachment point for colors.
 * @param name Name of the variable in shader.
 * @param position Position of the buffer.
 * @return Return false if given name is already added, true otherwise.
 */
bool MetaShaderProg::addColor(uint position, QString name)
{
    if(colorsAttach.values().contains(name))
        return false;

    if(colorsAttach.contains(position))
        return false;

    colorsAttach.insert(position, name);
    return true;
}

/**
 * @brief MetaShaderProg::addTexCoord Add new attachment point for texture coordinates.
 * @param name Name of the variable in shader.
 * @param position Position of the buffer.
 * @return Return false if given name is already added, true otherwise.
 */
bool MetaShaderProg::addTexCoord(uint position, QString name)
{
    if(texCoordsAttach.values().contains(name))
        return false;

    if(texCoordsAttach.contains(position))
        return false;

    texCoordsAttach.insert(position, name);
    return true;
}

/**
 * @brief MetaShaderProg::addTexture Add new attachment point to texture in this shader program.
 * @param texture Name of texture we want to use here.
 * @param point Point what we want to add. Must be unique.
 * @return Return false if given texture is already attached (it won't be attached),
 * true otherwise.
 */
bool MetaShaderProg::addTexture(QString point, QString texture)
{
    if(texturesAttach.contains(point))
    {
        return false;
    }

    texturesAttach.insert(point, texture);

    /*
    QString texName = texturesAttach.value(point,QString());

    if(texName.isEmpty())
    {
        texturesAttach.insert(point, texture);
        return true;
    }

    if(texName != texture)
    {
        texturesAttach.insert(point,texture);
    }
    */

    return true;
}

/**
 * @brief MetaShaderProg::setNewTexture Set new texture value.
 * @param point Texture attachment point where we want set texture.
 * @param texture Texture what we want to attach.
 * @return Return false if attachment point is missing, true otherwise.
 */
bool MetaShaderProg::setNewTexture(const QString point, const QString texture)
{
    if(!texturesAttach.contains(point))
        return false;

    texturesAttach.insert(point, texture);

    return true;
}

/**
 * @brief MetaShaderProg::removeColor Remove color attachment point from this shader program.
 * @param name Name of the color attachment point we want to remove.
 */
void MetaShaderProg::removeColor(const QString name)
{
    QMutableHashIterator<uint,QString> it(colorsAttach);

    while(it.hasNext())
    {
        it.next();

        if(it.value() == name)
        {
            it.remove();
            return;
        }
    }
}

/**
 * @brief MetaShaderProg::removeColor Remove color attachment in given position.
 * @param position Position of the attached buffer.
 */
void MetaShaderProg::removeColor(const uint position)
{
    colorsAttach.remove(position);
}

/**
 * @brief MetaShaderProg::removeTexCoord Remove texture coordinate attachment point from this shader program.
 * @param name Name of the color attachment point we want to remove.
 */
void MetaShaderProg::removeTexCoord(const QString name)
{
    QMutableHashIterator<uint,QString> it(texCoordsAttach);

    while(it.hasNext())
    {
        it.next();

        if(it.value() == name)
        {
            it.remove();
            return;
        }
    }

}

/**
 * @brief MetaShaderProg::removeTexCoord Remove texture coordinate in given buffer position.
 * @param position Position in buffer list.
 */
void MetaShaderProg::removeTexCoord(const uint position)
{
    texCoordsAttach.remove(position);
}

/**
 * @brief MetaShaderProg::removeTexture Remove texture with given name from this shader program.
 * @param point Name of the texture we want to remove.
 */
void MetaShaderProg::removeTexture(const QString point)
{
    texturesAttach.remove(point);
}

/**
 * @brief MetaShaderProg::removeTexturePoint Remove texture point from given texture.
 * @param textureName Texture from where we want remove attachment point.
 * @param pointName Name of attachment point we want to remove.
 */
/*
void MetaShaderProg::removeTexturePoint(const QString pointName, const QString textureName)
{
    QString tName = texturesAttach.value(pointName);

    QSet<QString>* list = texturesAttach.value(textureName);

    if(list->contains(pointName))
    {
        list->remove(pointName);

        if(list->isEmpty())
        {
            texturesAttach.remove(textureName);
            delete list;
        }
    }

}
*/

/**
 * @brief MetaShaderProg::isColors Test if any colors attachment point is set.
 * @return Return true if some attachment for colors exists, false otherwise.
 */
bool MetaShaderProg::isColors() const
{
    return !colorsAttach.isEmpty();
}

/**
 * @brief MetaShaderProg::isTexCoords Test if any texture coordinates attachment point is set.
 * @return Return true if some attachment for texture coordinates exists, false otherwise.
 */
bool MetaShaderProg::isTexCoords() const
{
    return !texCoordsAttach.isEmpty();
}

/**
 * @brief MetaShaderProg::isTexturesAttachment Test if any textures attachmet point is set.
 * @return Return true if some attachment for texture exists, false otherwise.
 */
bool MetaShaderProg::isTexturesAttachment() const
{
    return texturesAttach.isEmpty();
}

/**
 * @brief MetaShaderProg::isColor Test if color attachment with given name exists.
 * @param name Name of the color attachment.
 * @return Return true if this color attachment exists, false otherwise.
 */
bool MetaShaderProg::isColor(const QString name) const
{
    foreach(QString colName, colorsAttach.values())
    {
        if(colName == name)
            return true;
    }

    return false;
}

/**
 * @brief MetaShaderProg::isColor Test if color attachment with given position exists.
 * @param position Position of this attachment in color buffer list.
 * @return Return true if attachment exists, false otherwise.
 */
bool MetaShaderProg::isColor(const uint position) const
{
    return colorsAttach.contains(position);
}

/**
 * @brief MetaShaderProg::isTexCoord Test if texture coordinate attachment with given name exists.
 * @param name Name of the texture coordinate attachment.
 * @return Return true if this texture coordinate attachment exists, false otherwise.
 */
bool MetaShaderProg::isTexCoord(const QString name) const
{
    foreach(QString texName, texCoordsAttach.values())
    {
        if(texName == name)
            return true;
    }

    return false;
}

/**
 * @brief MetaShaderProg::isTexCoord Test if texture coordinate attachment with position in buffer list exists.
 * @param position Position in texture coordinate buffer list.
 * @return Return true if this texture coordinate attachment exists, false otherwise.
 */
bool MetaShaderProg::isTexCoord(const uint position) const
{
    return texCoordsAttach.contains(position);
}

/**
 * @brief MetaShaderProg::isTextureAttach Test if this point and texture is set.
 * @param point Attachment point for texture (sampler).
 * @param texture Texture to attach.
 * @return Return true if texture is attached to point, false otherwise.
 */
bool MetaShaderProg::isTextureAttach(const QString point, const QString texture)
{
    QString name = texturesAttach.value(point);

    if(name == texture)
        return true;

    return false;
}

/**
 * @brief MetaShaderProg::getName Return name of this program
 * @return program name
 */
QString MetaShaderProg::getName() const
{
    return programName;
}

/**
 * @brief MetaShaderProg::isValid Test if this shader program is valid for drawing. Test only if needed shaders are set.
 * @return Return true if fragment and vertex shaders are set, false otherwise.
 */
bool MetaShaderProg::isValid() const
{
    if(fragment.isEmpty())
        return false;

    if(vertex.isEmpty())
        return false;

    return true;
}


/** NON MEMBER of CLASS **/


/**
 * @brief operator << Operator for serialization of MetaShaderProg class
 * @param stream Stream for serialization
 * @param program Shader program for serialization
 * @return For easy serialization of more objects
 */
QDataStream & operator<< (QDataStream& stream, const MetaShaderProg& program)
{
    qDebug() << "Saving shader program " << program.programName;

    /*
    stream << program.programName;
    stream << program.vertex;
    stream << program.fragment;
    */

    program.saveProgram(stream, program);

    return stream;
}

/**
 * @brief operator >> Operator for deserialization of MetaShaderProg class
 * @param stream Stream for deserialization
 * @param program Shader program for deserialization
 * @return For easy deserialization of more objects
 */
QDataStream & operator>> (QDataStream& stream, MetaShaderProg& program)
{
    /*
    stream >> program.programName;
    stream >> program.vertex;
    stream >> program.fragment;
    */

    program.loadProgram(stream, program);



    return stream;
}

/**
 * @brief MetaShaderProg::textureNameChanged If texture name is changed we need to change it here too.
 * @param oldName What was the texture name before change.
 * @param newName What is the name now.
 */
void MetaShaderProg::textureNameChanged(QString oldName, QString newName)
{
    QList<QString> list = texturesAttach.keys();

    foreach(QString point, list)
    {
        if(texturesAttach.value(point) == oldName)
            texturesAttach.insert(point, newName);
    }
}

/**
 * @brief operator << For serializing this class. Save program to datastream.
 * @param stream Where to serialize.
 * @param program What serialize.
 * @return Stream for another use.
 */
QDataStream &operator <<(QDataStream &stream, const MetaShaderProg *program)
{
    program->saveProgram(stream, *program);

    return stream;
}

/**
 * @brief operator >> For serializing this class. Load from datastream to program.
 * @param stream From where we serialize.
 * @param program Where to load.
 * @return Stream for another use.
 */
QDataStream &operator >>(QDataStream &stream, MetaShaderProg*& program)
{
    program = new MetaShaderProg();

    program->loadProgram(stream, *program);

    return stream;
}

/**
 * @brief MetaShaderProg::saveProgram Save shader program to stream.
 * @param stream Where we want serialize.
 * @param program What to save.
 */
void MetaShaderProg::saveProgram(QDataStream &stream, const MetaShaderProg &program)
{
    qDebug() << "Saving shader program " << program.programName;

    stream << program.programName;
    stream << program.vertex;
    stream << program.fragment;
    stream << program.verticesAttach;
    stream << program.normalsAttach;
    stream << program.colorsAttach;
    stream << program.texCoordsAttach;
    stream << program.texturesAttach;
}

/**
 * @brief MetaShaderProg::loadProgram Load program from stream.
 * @param stream From where we want load.
 * @param program Where to load.
 */
void MetaShaderProg::loadProgram(QDataStream &stream, MetaShaderProg &program)
{
    qDebug() << "Loading shader program " << program.programName;

    stream >> program.programName;
    stream >> program.vertex;
    stream >> program.fragment;
    stream >> program.verticesAttach;
    stream >> program.normalsAttach;
    stream >> program.colorsAttach;
    stream >> program.texCoordsAttach;
    stream >> program.texturesAttach;
}
