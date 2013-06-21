#ifndef OGLWINDOW_H
#define OGLWINDOW_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QGLBuffer>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QTimer>
#include <QVariant>
#include <QList>
#include <QStack>
#include "infomanager.h"
#include "logeditor.h"
#include "model_work/storage/modelnode.h"
#include "storage/gltexture.h"
#include "profiling/timequerystorage.h"
#include "tools/datatimer.h"

/**
  Class for working with OpenGL
  */
class OGLwindow : public QGLWidget
{
    Q_OBJECT
public:
    explicit OGLwindow(QGLFormat &format, QTextEdit *edit, QWidget *parent);
    ~OGLwindow();
    bool loadShaders(const MetaShaderProg *prog);

    const TimeQueryStorage* getTimeQuery(const QString progName);
    QList<const TimeQueryStorage*> getTimeQueries();

    void invalidateRender();
    
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    QString checkError();

    //work with shader uniform variables
    bool setShaderUniform(QString progName, bool printWarning = false);
    void attachShaderUniform(const UniformVariable *u, int loc, QGLShaderProgram *prog, bool printWarnings);
    void createUniformTimers();
    void resetUniformTimers();

    void toggleAll(int buttonId);

    bool createNewBuffers();
    bool attachAttribBuffers(Mesh *mesh, const MetaShaderProg *program, bool writeErrors = false);
    bool attachTextures(const MetaShaderProg *program, bool writeErrors = false);
    bool loadAllShaders(QList<const MetaShaderProg *> programs);

    bool createTextures();

    bool isRenderValid();
    /**
     * @brief isShProgValid Test shader program if is valid, testing is with internal database.
     * @param progName Shader program name.
     * @return Return true if we can use this shader program, false otherwise.
     */
    inline bool isShProgValid(const QString progName) {return programs.contains(progName);}
    void recursiveDraw(ModelNode *node);
    bool setNewSettings();
    void removeSettings();

    GLint createQuery(const QString progName);
    void testQuery();
    void getQueryResults();
    void removeQueries();

    void setMVP(QString programName, QMatrix4x4 modelMatrix);
    void testProjection();
    void testView();

    QMatrix4x4 calculateMultiplyMat4x4(const UniformVariable &variable, bool showErrors);
    QMatrix3x3 calculateMultiplyMat3x3(const UniformVariable &variable, bool showErrors);
    QMatrix2x2 calculateMultiplyMat2x2(const UniformVariable &variable, bool showErrors);

    QVector4D calculateMultiplyVec4(const UniformVariable &variable, bool showErrors);
    QVector3D calculateMultiplyVec3(const UniformVariable &variable, bool showErrors);
    QVector2D calculateMultiplyVec2(const UniformVariable &variable, bool showErrors);

    GLenum convertToGLEnum(SettingsStorage::BLEND_FUNCTION func) const;


    template <typename T>
    /**
     * @brief calculateMultiplyInt Calculate multiply scalar size variable.
     * @param variable Multiply variable mode.
     * @param showErrors Print errors or warnings.
     * @return Calculated variable.
     */
    T calculateMultiplyScalar(const UniformVariable &variable, bool showErrors)
    {
        if(!variable.isMultiplyMode())
        {
            if(showErrors)
                log.addUniformError(tr("Error uniform variable %1 is not in multiply mode").arg(variable.getName()));

            return 0;
        }

        UniformTypes::UNIFORM_TYPES type = variable.getScalarType();
        QString name = variable.getName();
        T result = 1;
        MetaProject* proj = infoM->getActiveProject();

        QList<QVariant> list = variable.getValues();

        foreach(QVariant var, list)
        {
            UniformVariable* u = proj->getUniformVariable(var.toString());

            if(!testMultiplyVar(name,var.toString(),u,showErrors))
            {
                return 0;
            }

            if(u->getUniformSize() != UniformTypes::SCALAR)
            {
                if(showErrors)
                    log.addUniformError(tr("When computing multiply mode uniform variable %1, %2 is not Matrix 4x4!")
                                        .arg(name, var.toString()));

                return 0;
            }

            switch(type)
            {
            case UniformTypes::FLOAT:
                result *= u->getValueFloat();
                break;

            case UniformTypes::INT:
                result *= u->getValueInt();
                break;

            case UniformTypes::UINT:
                result *= u->getValueUInt();
                break;

            default:
                result *= u->getValueFloat();
            }
        }

        return result;
    }

    bool testMultiplyVar(QString name, QString innerVar, UniformVariable *var, bool showErrors);

private:
    bool prepareShaderProgram(const QString& vertexShaderPath, const QString& fragmentShaderPath);
    InfoManager *infoM;
    GLfloat rotx, roty, zoomZ;
    QMatrix4x4 view;
    QMatrix4x4 projection;
    QStack<QMatrix4x4> mvpStack;
    QStack<QMatrix4x4> nodeTransStack;
    QList<QMatrix4x4> nodeTransList;

    GLuint vao;

    //int mvp_loc;
    QHash<QString,QGLShaderProgram *> shaders;
//    QHash<QString,QGLShaderProgram *> backupShaders;
    QHash<QString,const MetaShaderProg*> programs;
    QList<QGLBuffer*> vertexBuffers;
    QList<QGLBuffer*> colorBuffers;
    QList<QGLBuffer*> texCoordBuffers;
    QList<QGLBuffer*> indexBuffers;
    QList<QGLBuffer*> normalBuffers;
    QString logReport;
    ModelNode* rootNode;
    QHash<QString,GLTexture*> textures;
    bool canRender;
    bool showErrors;
    bool availableSettings;
    bool isDrawPaused;

    //bool rotate[4];
    //bool zoom[2];

    // for logging information
    LogEditor log;

    // for time measurement
    //QTimer* measureTimer;
    //GLuint timeQuery;
    //bool isQuerySet;

    QHash<QString,TimeQueryStorage*> profiles;
    QList<DataTimer*> uniformTimers;
    QList<UniformVariable*> timeUniforms;
    QList<UniformVariable*> pressedUniforms;

    bool buttonPressedField[15];
    bool testingVar;

signals:
    //void newMeasure(double time);
    void queryCreated(const QString progName);
    void queryDestroyed(const QString progName);
    
private slots:
    void rotTimeout();
    void incUnifTimeTimers(long id);
    void incUnifActionPressedTimers(long id);

public slots:
    //void loadActiveShaders();
    void runShaders();
    void loadNewModel();
    void reloadShaderPrograms();
    void newTextures();
    void newUniformValues();

    void pauseDrawing(bool pause);

    void buttonPressed();
    void buttonReleased();

    void setOpenGLSettings();

    //void rotateObjectBtnPressed();
    //void rotateObjectBtnReleased();
    //void zoomObjectBtnPressed();
    //void zoomObjectBtnReleased();
    
};

#endif // OGLWINDOW_H
