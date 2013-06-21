#ifndef METASHADERPROG_H
#define METASHADERPROG_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QSet>
#include <QHash>
#include "texture/texturestorage.h"

class MetaShaderProg : public QObject
{
    Q_OBJECT

public:
    MetaShaderProg(QObject* parent = NULL); //default constructor
    MetaShaderProg(QString name, QDir projectBase, QObject* parent = NULL);
    MetaShaderProg(QString vertex, QString fragment, QString name, QDir projectBase, QObject* parent = NULL);

    const MetaShaderProg *copy();

    void setProgramName(QString name);
    void setProjectBaseDir(QDir base);
    void setVertexShader(QString shader);
    void setFragmentShader(QString shader);

    QString getVertexShader() const;
    QString getFragmentShader() const;
    QString getVertexAbsolutePath() const;
    QString getFragmentAbsolutePath() const;
    QStringList getShaders() const;

    // get and set attachments

    /**
     * @brief getVerticesAttach Get vertices attachment point for this shader program.
     * @return Return name of the vertices attachment point.
     */
    inline QString getVerticesAttach() const {return verticesAttach;}

    /**
     * @brief getNormalsAttach Get normals attachment point for this shader program.
     * @return Return name of the normals attachment point.
     */
    inline QString getNormalsAttach() const {return normalsAttach;}

    /**
     * @brief setVerticesAttach Set vertices attachment point for this shader program.
     * @param varName Return name of the vertices attachment point.
     */
    inline void setVerticesAttach(QString varName) {verticesAttach = varName;}

    /**
     * @brief setNormalsAttach Set normals attachment point for this shader program.
     * @param varName Return name of the normals attachment point.
     */
    inline void setNormalsAttach(QString varName) {normalsAttach = varName;}

    /**
     * @brief isVertices Test if vertices attachment point is set in this shader program.
     * @return Return true if vertices attachment point is set, false otherwise.
     */
    inline bool isVertices() const {return !verticesAttach.isEmpty();}

    /**
     * @brief isNormals Test if normals attachment point is set in this shader program.
     * @return Return true if normals attachment point is set, false otherwise.
     */
    inline bool isNormals() const {return !normalsAttach.isEmpty();}

    // work with lists
    QStringList getColorNames() const;
    QStringList getTexCoordNames() const;
    //QStringList getTextureAttachment(const QString textureName);
    QStringList getTextureNames() const;
    QStringList getTexturePoints() const;
    QString getTexture(QString pointName) const;

    QString getColor(uint position) const;
    uint getColor(QString name) const;
    QString getTexCoord(uint position) const;
    uint getTexCoord(QString name) const;
    QList<uint> getColorPositions() const;
    QList<uint> getTexCoordPositions() const;

    bool addColor(uint position, QString name);
    bool addTexCoord(uint position, QString name);
    bool addTexture(QString point, QString texture);

    bool setNewTexture(const QString point, const QString texture);

    void removeColor(const QString name);
    void removeColor(const uint position);
    void removeTexCoord(const QString name);
    void removeTexCoord(const uint position);
    void removeTexture(const QString point);
    //void removeTexturePoint(const QString pointName, const QString textureName);

    bool isColors() const;
    bool isTexCoords() const;
    bool isTexturesAttachment() const;

    bool isColor(const QString name) const;
    bool isColor(const uint position) const;
    bool isTexCoord(const QString name) const;
    bool isTexCoord(const uint position) const;
    bool isTextureAttach(const QString point, const QString texture);

    QString getName() const;

    bool isValid() const;

    // for serializing
    friend QDataStream & operator<< (QDataStream& stream, const MetaShaderProg& program);
    friend QDataStream & operator>> (QDataStream& stream, MetaShaderProg& program);

    friend QDataStream & operator<< (QDataStream& stream, const MetaShaderProg* program);
    friend QDataStream & operator>> (QDataStream& stream, MetaShaderProg*& program);

private:
    static void saveProgram(QDataStream& stream, const MetaShaderProg& program);
    static void loadProgram(QDataStream& stream, MetaShaderProg& program);

signals:
    void nameChanged(QString oldName, QString newName);

public slots:
    void textureNameChanged(QString oldName, QString newName);

private:
    QString programName;
    QString vertex;
    QString fragment;
    QString verticesAttach;
    QString normalsAttach;
    QHash<uint,QString> colorsAttach;
    QHash<uint,QString> texCoordsAttach;
    QHash<QString,QString> texturesAttach;
    QDir baseDir;
};

// for serializing
QDataStream & operator<< (QDataStream& stream, const MetaShaderProg& program);
QDataStream & operator>> (QDataStream& stream, MetaShaderProg& program);

QDataStream & operator<< (QDataStream& stream, const MetaShaderProg* program);
QDataStream & operator>> (QDataStream& stream, MetaShaderProg*& program);

#endif // METASHADERPROG_H
