#ifndef METASHADER_H
#define METASHADER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>

/**
 * @brief The MetaShader class Class for storing information about shader.
 */
class MetaShader : public QObject
{
    Q_OBJECT

public:
    enum SHADERTYPE{VERTEX = 0, FRAGMENT = 1};
    MetaShader(QObject* parent = NULL);
    MetaShader(QString relativePath , SHADERTYPE type, QObject* parent = NULL);
    MetaShader(QFile file, QDir projectBaseDir, SHADERTYPE type, QObject* parent = NULL);
    MetaShader(QString file, QDir projectBaseDir, SHADERTYPE type, QObject* parent = NULL);

    QString getShader();
    QString getShaderName();
    SHADERTYPE getType();

    friend QDataStream & operator<< (QDataStream& stream, const MetaShader& shader);
    friend QDataStream & operator>> (QDataStream& stream, MetaShader& shader);
    friend QDataStream & operator<< (QDataStream& stream, const MetaShader* shader);
    friend QDataStream & operator>> (QDataStream& stream, MetaShader*& shader);

    //friend QDataStream& SerializeFuncs::writeShaderObject(QDataStream &stream, const MetaShader &shader);
    //friend QDataStream& SerializeFuncs::readShaderObject(QDataStream &stream, MetaShader &shader);

protected:
    static QDataStream& writeObject(QDataStream &stream, const MetaShader &shader);
    static QDataStream& readObject(QDataStream &stream, MetaShader &shader);

private:
    QString file;
    SHADERTYPE type;
};

// for serializing
QDataStream & operator<< (QDataStream& stream, const MetaShader& shader);
QDataStream & operator>> (QDataStream& stream, MetaShader& shader);
QDataStream & operator<< (QDataStream& stream, const MetaShader* shader);
QDataStream & operator>> (QDataStream& stream, MetaShader*& shader);

#endif // METASHADER_H
