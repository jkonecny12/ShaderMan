#include "oglwindow.h"
#include "meta_data/metashaderprog.h"
#include "model_work/storage/mesh.h"
#include "model_work/storage/model.h"

#define XM 9
#define XP 10

#define YM 11
#define YP 12

#define ZM 13
#define ZP 14

#define MVP_CHAR "mvp"
#define PROJECTION_CHAR "projectionM"
#define VIEW_CHAR "viewM"
#define MODEL_CHAR "modelM"

/**
 * @brief OGLwindow::OGLwindow Create OpenGL window after this creation initializeGL method is called.
 * @param format OpenGL context format, OpenGL version is set here.
 * @param edit Logging widget in program.
 * @param parent Parent of this widget.
 */
OGLwindow::OGLwindow(QGLFormat& format, QTextEdit *edit, QWidget *parent) :
    QGLWidget(format, parent)
{
    testingVar = true;

    for(int i = 0; i < 15; ++i)
        buttonPressedField[i] = false;

    isDrawPaused = false;

    rootNode = NULL;

    projection.perspective(60,4.0/3.0,0.1,500.0);
    //projection.ortho(-20,20,-20,20,0.1,500);
    zoomZ = -4.0;
    view.translate(0.0,0.0,zoomZ);

    mvpStack.push(projection);
    //mvpStack.push(projection*view);

    QMatrix4x4 mat;
    mat.setToIdentity();
    mvpStack.push(mvpStack.top() * mat);

    log.setLogWindow(edit);

    rotx = 45.0f;
    roty = 0.0f;
    QTimer* timer = new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT(rotTimeout()));
    timer->start(35);

    canRender = false;
    showErrors = true;
    availableSettings = false;

    infoM = InfoManager::getInstance();
}

/**
 * @brief OGLwindow::~OGLwindow Clean memory
 */
OGLwindow::~OGLwindow()
{
    qDeleteAll(shaders);
    qDeleteAll(vertexBuffers);
    qDeleteAll(colorBuffers);
    qDeleteAll(texCoordBuffers);
    qDeleteAll(indexBuffers);
    qDeleteAll(normalBuffers);
    qDeleteAll(programs);
    qDeleteAll(textures);

    removeQueries();
    qDeleteAll(profiles);
    qDeleteAll(uniformTimers);
}

/**
 * @brief OGLwindow::loadShaders Load shaders from shader program.
 * @param prog Shader program from where we get shaders.
 * @return Return true if shader is compiled and linked correctly, otherwise false.
 */
bool OGLwindow::loadShaders(const MetaShaderProg* prog)
{
    log.newCompiling(); // erase log
    bool isFailed = false;
    bool shaderMissing = false;
    MetaProject* actProj = infoM->getActiveProject();

    if(!prog->isValid())
    {
        log.addToLog(tr("Shader program %1 canno't be used. No valid vertex and fragment shader are set.")
                     .arg(prog->getName()));
        return false;
    }

    //newShader = new QGLShaderProgram(this); // create new one
    //QHash<QString,QGLShaderProgram *> backupShaders = shaders;

    //shaders.clear();
    QGLShaderProgram* oglShader = new QGLShaderProgram(this);
    //shaders.insert(prog->getName(), oglShader);

    //add vertex shader to shader
    MetaShader* vertex = actProj->getVertexShader(prog->getVertexShader());
    if(vertex == NULL)
    {
        log.addVertexLog(tr("Vertex shader '%1' from shader program '%2' missing!")
                         .arg(prog->getVertexShader(), prog->getName()));
        isFailed = true;
        shaderMissing = true;
    }
    else if(!oglShader->addShaderFromSourceFile(QGLShader::Vertex, actProj->getVertexFilePath(vertex->getShader())))
    {
        log.addVertexLog(oglShader->log());
        isFailed = true;
    }

    //add fragment shader to shader
    MetaShader* fragment = actProj->getFragmentShader(prog->getFragmentShader());
    if(fragment == NULL)
    {
        log.addFragmentLog(tr("Fragment shader '%1' from shader program '%2' missing!")
                           .arg(prog->getFragmentShader(), prog->getName()));
        isFailed = true;
        shaderMissing = true;
    }
    else if(!oglShader->addShaderFromSourceFile(QGLShader::Fragment, actProj->getFragmentFilePath(fragment->getShader())))
    {
        log.addFragmentLog(oglShader->log());
        isFailed = true;
    }

    if(!shaderMissing)
    {
        // link shader program
        if(!oglShader->link())
        {
            log.addLinkLog(oglShader->log());
            //return false;
            isFailed = true;
        }
    }

    if(isFailed)
    {
        delete oglShader;
        return false;
    }
    else
    {
        if(shaders.contains(prog->getName()))
        {
            delete shaders.value(prog->getName());
        }

        shaders.insert(prog->getName(), oglShader);
    }

    return true;
}

/**
 * @brief OGLwindow::getTimeQuery Get time query object reference. For measuring drawing time.
 * @param progName Shader program name, what we want to measure.
 * @return Return time query measure object.
 */
const TimeQueryStorage *OGLwindow::getTimeQuery(const QString progName)
{
    return profiles.value(progName,NULL);
}

/**
 * @brief OGLwindow::getTimeQueries Get all time query measure objects. For measuring drawing time of all shader programs.
 * @return Return list of time query measure objects.
 */
QList<const TimeQueryStorage *> OGLwindow::getTimeQueries()
{
    QList<const TimeQueryStorage *> ret;

    foreach(TimeQueryStorage* s, profiles)
    {
        ret.append(s);
    }

    return ret;
}

/**
 * @brief OGLwindow::invalidateRender Invalidate current render. Remove all saved information and set flag to not render.
 */
void OGLwindow::invalidateRender()
{
    canRender = false;
    removeSettings();
}

/**
 * @brief OGLwindow::setShaderUniform Set shader uniform to shaders for given shader program.
 * @param progName Shader program name where is the shader.
 * @param printWarning If true print missing variables to log, else do not print.
 * @return Return true if uniform variables are correctly set to shader, otherwise return false.
 */
bool OGLwindow::setShaderUniform(QString progName, bool printWarning)
{
    QList<UniformVariable*> unif = infoM->getActiveProject()->getUniformVariables(progName);
    QGLShaderProgram* glprog = shaders.value(progName);
    glprog->bind();

    foreach(UniformVariable* u, unif)
    {
        QStringList variables = u->getAttachedVariables(progName);

        foreach(QString var, variables)
        {
            int loc = glprog->uniformLocation(var);

            if(loc < 0)
            {
                if(printWarning)
                    log.addUniformError("Shader variable '" + var + "' didn't exists.");
            }
            else
            {
                attachShaderUniform(u,loc,glprog,printWarning);
            }
        }
    }

    return true;
}

/**
 * @brief OGLwindow::attachShaderUniform Attach uniform variable to shader program.
 * @param u Uniform variable we want to attach.
 * @param loc Location of uniform variable in shader program.
 * @param prog OpenGL shader program.
 */
void OGLwindow::attachShaderUniform(const UniformVariable *u, int loc, QGLShaderProgram* prog, bool printWarnings)
{
    UniformTypes::UNIFORM_TYPES size = u->getUniformSize();
    UniformTypes::UNIFORM_TYPES type = u->getScalarType();

    switch(size){
    case UniformTypes::SCALAR:
        switch(type)
        {
        case UniformTypes::INT:
            if(u->isMultiplyMode())
            {
                prog->setUniformValue(loc,calculateMultiplyScalar<int>(*u,printWarnings));
            }
            else if(u->isArray())
            {
                int *values = u->getValuesInt();
                prog->setUniformValueArray(loc, values, u->getVarCount());
                delete values;
            }
            else
                prog->setUniformValue(loc,u->getValueInt());
            break;
        case UniformTypes::UINT:
            if(u->isMultiplyMode())
            {
                prog->setUniformValue(loc,calculateMultiplyScalar<uint>(*u,printWarnings));
            }
            else if(u->isArray())
            {
                uint *values = u->getValuesUInt();
                prog->setUniformValueArray(loc, values, u->getVarCount());
                delete values;
            }
            else
                prog->setUniformValue(loc,u->getValueUInt());
            break;
        case UniformTypes::FLOAT:
            if(u->isMultiplyMode())
            {
                prog->setUniformValue(loc,calculateMultiplyScalar<float>(*u,printWarnings));
                break;
            }
            else if(u->isArray())
            {
                float *values = u->getValuesFloat();
                prog->setUniformValueArray(loc, values, u->getVarCount(), 1);
                delete values;
            }
            else
                prog->setUniformValue(loc,u->getValueFloat());
            break;
            /*
        case DOUBLE:
            prog->setUniformValue(loc,static_cast<float>(u->getValueDouble()));
            break;
            */
        default:
            prog->setUniformValue(loc,u->getValueInt());
            break;
        }

        break;

    case UniformTypes::VEC2:
        if(u->isMultiplyMode())
        {
            prog->setUniformValue(loc,calculateMultiplyVec2(*u,printWarnings));
        }
        else if(u->isArray())
        {
            QVector2D *values = u->getValuesVec2D();
            prog->setUniformValueArray(loc, values, u->getVarCount());
            delete values;
        }
        else
            prog->setUniformValue(loc,u->getValueVec2D());
        break;
    case UniformTypes::VEC3:
        if(u->isMultiplyMode())
        {
            prog->setUniformValue(loc,calculateMultiplyVec3(*u,printWarnings));
        }
        else if(u->isArray())
        {
            QVector3D *values = u->getValuesVec3D();
            prog->setUniformValueArray(loc, values, u->getVarCount());
            delete values;
        }
        else
            prog->setUniformValue(loc,u->getValueVec3D());
        break;
    case UniformTypes::VEC4:
        if(u->isMultiplyMode())
        {
            prog->setUniformValue(loc,calculateMultiplyVec4(*u,printWarnings));
        }
        else if(u->isArray())
        {
            QVector4D *values = u->getValuesVec4D();
            prog->setUniformValueArray(loc, values, u->getVarCount());
            delete values;
        }
        else
            prog->setUniformValue(loc,u->getValueVec4D());
        break;

    case UniformTypes::MAT2:
        if(u->isMultiplyMode())
        {
            prog->setUniformValue(loc,calculateMultiplyMat2x2(*u,printWarnings));
        }
        else if(u->isArray())
        {
            QMatrix2x2 *values = u->getValuesMat2x2();
            prog->setUniformValueArray(loc, values, u->getVarCount());
            delete values;
        }
        else
            prog->setUniformValue(loc,u->getValueMat2x2());
        break;
    case UniformTypes::MAT2X3:
        break;
    case UniformTypes::MAT2X4:
        break;
    case UniformTypes::MAT3:
        if(u->isMultiplyMode())
        {
            prog->setUniformValue(loc,calculateMultiplyMat3x3(*u,printWarnings));
        }
        else if(u->isArray())
        {
            QMatrix3x3 *values = u->getValuesMat3x3();
            prog->setUniformValueArray(loc, values, u->getVarCount());
            delete values;
        }
        else
            prog->setUniformValue(loc,u->getValueMat3x3());
        break;
    case UniformTypes::MAT3X2:
        break;
    case UniformTypes::MAT3X4:
        break;
    case UniformTypes::MAT4:
        if(u->isMultiplyMode())
        {
            prog->setUniformValue(loc,calculateMultiplyMat4x4(*u,printWarnings));
        }
        else if(u->isArray())
        {
            QMatrix4x4 *values = u->getValuesMat4x4();
            prog->setUniformValueArray(loc, values, u->getVarCount());
            delete values;
        }
        else
            prog->setUniformValue(loc,u->getValueMat4x4());
        break;
    case UniformTypes::MAT4X2:
        break;
    case UniformTypes::MAT4X3:
        break;

    default:
        break;
    }
}

/**
 * @brief OGLwindow::createUniformTimers Create timers for ActionPressed and Time special variable.
 * Append these and Action uniform variables to special lists for better working with it.
 */
void OGLwindow::createUniformTimers()
{
    /*
    qDeleteAll(uniformTimers);
    uniformTimers.clear();
    timeUniforms.clear();
    pressedUniforms.clear();
    */

    /*
    QStringList progNames = infoM->getActiveProject()->getProgramNames();

    foreach(QString name, progNames)
    {
    */

    QList<UniformVariable*> unifs = infoM->getActiveProject()->getUniformVariables();

    foreach(UniformVariable* u, unifs)
    {
        QList<long> timers = u->getTimeTimers();

        foreach(long t, timers)
        {
            DataTimer* timer = new DataTimer(t,DataTimer::TIME,this);
            timer->setInterval(t);
            timer->setSingleShot(false);
            connect(timer,SIGNAL(timeout(long)),this,SLOT(incUnifTimeTimers(long)));

            uniformTimers.append(timer);
            timer->start();
        }

        if(!timers.isEmpty())
            timeUniforms.append(u);

        // action pressed
        QList<long> pressedTimers = u->getActionPressedTimers();

        foreach(long t, pressedTimers)
        {
            DataTimer* timer = new DataTimer(t, DataTimer::ACTION_PRESSED, this);
            timer->setInterval(t);
            timer->setSingleShot(false);
            connect(timer,SIGNAL(timeout(long)),this,SLOT(incUnifActionPressedTimers(long)));

            uniformTimers.append(timer);
            timer->start();
        }

        if(!pressedTimers.isEmpty())
            pressedUniforms.append(u);
    }
    //}
}

/**
 * @brief OGLwindow::resetUniformTimers Reset all uniform special variables to default values.
 * Reset default projection and view variable too.
 */
void OGLwindow::resetUniformTimers()
{
    // reset default variables
    projection.setToIdentity();
    projection.perspective(60,4.0/3.0,0.1,500.0);

    roty = 0.0;
    rotx = 0.0;
    zoomZ = -4.0;

    QMatrix4x4 rot;
    rot.setToIdentity();
    rot.rotate(roty,1.0f,0.0f);
    rot.rotate(rotx,0.0f,1.0f);

    QMatrix4x4 zoom;
    zoom.setToIdentity();
    zoom.translate(0.0,0.0,zoomZ);

    view.setToIdentity();
    view *= zoom * rot;

    // reset special variables
    QList<UniformVariable*> unifs = infoM->getActiveProject()->getUniformVariables();

    foreach(UniformVariable* u, unifs)
    {
        u->resetToDefaults();
    }
}

/**
 * @brief OGLwindow::toggleAll For Action special variable. Toggle setted values to next value, if button with buttonId is pressed.
 * @param buttonId Id of the button what we want toggle value.
 */
void OGLwindow::toggleAll(int buttonId)
{
    if(isDrawPaused)
        return;

    QList<UniformVariable*> unifs = infoM->getActiveProject()->getUniformVariables();

    foreach(UniformVariable* u, unifs)
    {
        u->toggleAction(buttonId);
    }
}

/**
 * @brief OGLwindow::createNewBuffers Create new OpenGL bufers for setted model.
 * @return Return false if some buffer canno't be bind or if vertices or indices are not found, true otherwise.
 */
bool OGLwindow::createNewBuffers()
{
    Model* model = infoM->getActiveProject()->getModel();

    const QList<Mesh*> list = model->getMeshes();

    foreach(Mesh* m, list)
    {
        // vertex buffer
        if(m->hasVertices())
        {
            QGLBuffer* vBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
            vBuffer->create();
            vBuffer->setUsagePattern(QGLBuffer::StaticDraw);

            if(!vBuffer->bind())
            {
                log.addBufferError("Could not bind vertex buffer");
                return false;
            }

            vBuffer->allocate(m->getVertices(),m->getSizeVertices());

            vertexBuffers.append(vBuffer);
            m->setVertexBufferIndex(vertexBuffers.lastIndexOf(vBuffer));
        }
        else
        {
            canRender = false;
            log.addBufferError("Could not render without vertices!");
            return false;
        }

        // normal buffer
        if(m->hasNormals())
        {
            QGLBuffer* nBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
            nBuffer->create();
            nBuffer->setUsagePattern(QGLBuffer::StaticDraw);

            if(!nBuffer->bind())
            {
                log.addBufferError("Could not bind normal buffer");
                return false;
            }

            nBuffer->allocate(m->getNormals(),m->getSizeNormals());

            normalBuffers.append(nBuffer);
            m->setNormalBufferIndex(normalBuffers.lastIndexOf(nBuffer));
        }

        // color buffer
        if(m->hasColors())
        {
            for(unsigned int i = 0; i < m->getColorBuffersCount(); ++i)
            {
                QGLBuffer* cBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
                cBuffer->create();
                cBuffer->setUsagePattern(QGLBuffer::StaticDraw);

                if(!cBuffer->bind())
                {
                    log.addBufferError(tr("Could not bind color buffer"));
                    return false;
                }

                cBuffer->allocate(m->getColors(i),m->getSizeColors(i));

                colorBuffers.append(cBuffer);
                m->addColorBufferIndex(colorBuffers.lastIndexOf(cBuffer));
            }
        }

        // texture coordinate buffer
        if(m->hasTexCoords())
        {
            for(unsigned int i = 0; i < m->getTexCoordBuffersCount(); ++i)
            {
                QGLBuffer* tBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
                tBuffer->create();
                tBuffer->setUsagePattern(QGLBuffer::StaticDraw);

                if(!tBuffer->bind())
                {
                    log.addBufferError(tr("Could not bind texture coordinate buffer!"));
                    return false;
                }

                tBuffer->allocate(m->getTexCoords(i), m->getSizeTexCoords(i));

                texCoordBuffers.append(tBuffer);
                m->addTexCoordBufferIndex(texCoordBuffers.lastIndexOf(tBuffer));
            }
        }

        // index buffer
        if(m->hasIndices())
        {
            QGLBuffer* iBuffer = new QGLBuffer(QGLBuffer::IndexBuffer);
            iBuffer->create();
            iBuffer->setUsagePattern(QGLBuffer::StaticDraw);

            if(!iBuffer->bind())
            {
                log.addBufferError("Could not bind index buffer");
                return false;
            }

            iBuffer->allocate(m->getIndices(),m->getSizeIndices());

            indexBuffers.append(iBuffer);
            m->setIndexBufferIndex(indexBuffers.lastIndexOf(iBuffer));
        }

    }

    return true;

}

/**
 * @brief OGLwindow::attachAttribBuffers Attach attributes from mesh to shader program.
 * @param mesh Mesh of the model.
 * @param program Shader program we want to use.
 * @param writeErrors If true write errors to logging window.
 * @return Return true if all buffer are binded correctly and vertices and indices exists, false otherwise.
 */
bool OGLwindow::attachAttribBuffers(Mesh* mesh, const MetaShaderProg* program, bool writeErrors)
{
    QGLBuffer* buffer;
    QGLShaderProgram* m_shader = shaders.value(program->getName());

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    if(!m_shader->bind())
    {
        if(writeErrors)
            log.addToLog("Could not bind shader program to context\n");

        return false;
    }

    // attach vertices
    if(mesh->hasVertices())
    {
        int loc = m_shader->attributeLocation(program->getVerticesAttach());
        if(loc != -1)
        {
            buffer = vertexBuffers.value(mesh->getVertexBufferIndex());
            buffer->bind();
            m_shader->setAttributeBuffer(loc,GL_FLOAT,0,3);
            m_shader->enableAttributeArray(loc);
        }
        else
        {
            if(writeErrors)
                log.addBufferError("Bad attachment location for vertices!");
            return false;
        }
    }
    else // canno't draw without vertices
    {
        log.addBufferError("Canno't attach vertices buffer");
        return false;
    }

    // attach colors
    if(mesh->hasColors() && program->isColors())
    {
        for(unsigned int i = 0; i < mesh->getColorBuffersCount(); ++i)
        {
            if(!program->isColor(i))
                continue;

            int loc = m_shader->attributeLocation(program->getColor(i));
            if(loc != -1)
            {
                buffer = colorBuffers.value(mesh->getColorBufferIndex(i));
                buffer->bind();
                m_shader->setAttributeBuffer(loc,GL_FLOAT,0,4);
                m_shader->enableAttributeArray(loc);
            }
            else
            {
                if(writeErrors)
                    log.addBufferError(tr("Bad attachment location for colors!"));
            }
        }
    }

    if(mesh->hasTexCoords() && program->isTexCoords())
    {
        for(unsigned int i = 0; i < mesh->getTexCoordBuffersCount(); ++i)
        {
            if(!program->isTexCoord(i))
                continue;

            int loc = m_shader->attributeLocation(program->getTexCoord(i));
            if(loc != -1)
            {
                buffer = texCoordBuffers.value(mesh->getTextCoordBufferIndex(i));
                buffer->bind();
                m_shader->setAttributeBuffer(loc,GL_FLOAT,0,2);
                m_shader->enableAttributeArray(loc);
            }
            else
            {
                if(writeErrors)
                    log.addBufferError(tr("Bad attachment location for texture coordinate %1!").arg(i));
            }
        }
    }

    // attach normals
    if(mesh->hasNormals() && program->isNormals())
    {
        int loc = m_shader->attributeLocation(program->getNormalsAttach());
        if(loc != -1)
        {
            buffer = normalBuffers.value(mesh->getNormalBufferIndex());
            buffer->bind();
            m_shader->setAttributeBuffer(loc,GL_FLOAT,0,3);
            m_shader->enableAttributeArray(loc);

            //qDebug() << "found normals attachment";
        }
        else
        {
            if(writeErrors)
                log.addBufferError("Bad attachment location for normals!\n");
        }

    }

    // attach indices
    if(mesh->hasIndices())
    {
        buffer = indexBuffers.value(mesh->getIndexBufferIndex());
        buffer->bind();
    }
    else
    {
        if(writeErrors)
            log.addBufferError("Canno't attach indices buffer");
        return false;
    }

    return true;
}

/**
 * @brief OGLwindow::attachTextures Attach textures for rendering to shader program.
 * @param program Shader program what we using for this drawing.
 * @param writeErrors If we want write errors to log window.
 * @return Return true if attaching textures is valid, false otherwise.
 */
bool OGLwindow::attachTextures(const MetaShaderProg *program, bool writeErrors)
{
    if(textures.isEmpty())
        return true;

    QGLShaderProgram* m_shader = shaders.value(program->getName());

    if(!m_shader->bind())
    {
        if(writeErrors)
            log.addToLog(tr("Could not bind shader program to context\n"));

        return false;
    }

    uint counter = 0;

    foreach(QString point, program->getTexturePoints())
    {
        QString name = program->getTexture(point);

        if(textures.contains(name))
        {
            int loc = m_shader->uniformLocation(point);

            if(loc != -1)
            {
                textures.value(name)->useTexture(loc,counter);
                ++counter;
            }
            else
            {
                if(writeErrors)
                    log.addToLog(tr("Bad attachment location %1 for texture %2!\n").arg(point, name));
            }
        }
    }

    return true;
}

/**
 * @brief OGLwindow::loadAllShaders Create all shader programs and set all shader uniform variables, if our shader program is valid.
 * @param programs Shader program with information how to create OpenGL shader.
 * @return Return true if shaders are created correctly and uniform variables are setted correctly, false otherwise.
 */
bool OGLwindow::loadAllShaders(QList<const MetaShaderProg *> programs)
{
    foreach(const MetaShaderProg* prog, programs)
    {
        if(!prog->isValid())
            return false;

        if(!loadShaders(prog))
            return false;

        if(!setShaderUniform(prog->getName()))
            return false;

    }

    return true;
}

/**
 * @brief OGLwindow::createTextures Create new OpenGL textures.
 * @return Return true if nothing bad happen, otherwise false.
 */
bool OGLwindow::createTextures()
{
    QStringList texList = infoM->getActiveProject()->getTextureNameList();

    // create GLTexture objects from TextureStorage objects
    foreach(QString texName, texList)
    {
        const TextureStorage* storage = infoM->getActiveProject()->getTexture(texName)->copy();

        if(!storage->exists())
        {
            log.addToCompiling(tr("Texture %1 missing an image file!\n").arg(storage->getName()));
            delete storage;
            continue;
        }

        textures.insert(storage->getName(), new GLTexture(storage));
    }

    // create OpenGL textures
    foreach(GLTexture* texture, textures.values())
    {
        if(!texture->createOGLTexture())
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief OGLwindow::isRenderValid Test if model can be rendered. It is if model is set, minimal one shader program is created and is valid.
 * @return Return false if one of the conditions is not correct, false otherwise.
 */
bool OGLwindow::isRenderValid()
{
    MetaProject* project = infoM->getActiveProject();

    if(project == NULL)
        return false;

    if(!project->isModelLoaded())
        return false;

    ModelNode* node = project->getModel()->getRootNode();

    if(node == NULL) // model is not set
        return false;

    //QList<MetaShaderProg *> list = infoM->getActiveProject()->getModel()->getAttachedPrograms();
    QStringList programs = project->getModel()->getAttachedPrograms();
    QList<MetaShaderProg *> list;

    foreach(QString prog, programs)
    {
        MetaShaderProg* shProg = project->getProgram(prog);

        if(shProg == NULL)
            return false;

        list.append(shProg);
    }

    if(list.isEmpty())
        return false;

    foreach(MetaShaderProg* prog, list)
    {
        if(!prog->isValid())
            return false;
    }

    return true;
}

/**
 * @brief OGLwindow::recursiveDraw Recursively draw 3D model.
 * @param node Actual node what we want to draw.
 */
void OGLwindow::recursiveDraw(ModelNode *node)
{
    if(isDrawPaused)
        return;

    if(!canRender || shaders.isEmpty() || availableSettings)
        return;

    QMatrix4x4 nodeTrans = node->getNodeTransformation();
    mvpStack.push(mvpStack.top() * nodeTrans);

    nodeTransList.append(nodeTrans);

    if(nodeTransStack.isEmpty())
        nodeTransStack.push(nodeTrans);
    else
        nodeTransStack.push(nodeTransStack.top() * nodeTrans);

    foreach(Mesh* mesh, *node->getNodeMeshes())
    {
        QString name = node->getShaderProgram(mesh);
        //MetaShaderProg* prog = infoM->getActiveProject()->getProgram(name);

        /*
        QString name;
        if(prog == NULL)
            name = infoM->getActiveProject()->getProgram()->getName();
        else
            name = prog->getName();
        */
        //QString name = node->getShaderProgram(mesh)->getName();

        if(!isShProgValid(name))
            continue;

        shaders.value(name)->bind();
        //shaders.value(node->getShaderProgram(mesh)->getName())->setUniformValue(mvp_loc,mvpStack.top());
        //shaders.value(name)->setUniformValue("mvp",mvpStack.top());
        setMVP(name, nodeTransStack.top());

        if(!setShaderUniform(name, showErrors))
            continue;

        //if(!attachAttribBuffers(mesh, node->getShaderProgram(mesh), showErrors))
        if(!attachAttribBuffers(mesh, programs.value(name), showErrors))
            continue;

        if(!attachTextures(programs.value(name), showErrors))
            continue;

        if(!availableSettings)
        {
            /*
            QString error = checkError();
            log.addToLog(error);
            if(!error.isEmpty())
                qDebug() << error;
            */
            GLint queryId = createQuery(name);

            //GLuint timeQuery;
            //for drawing time measurement
            //glGenQueries(1, &timeQuery);

            //qDebug() << "Query created" << timeQuery;
            //query->pushQuery(timeQuery);

            //glBeginQuery(GL_TIME_ELAPSED,timeQuery);

            if(queryId >= 0)
                glBeginQuery(GL_TIME_ELAPSED,queryId);

            glDrawElements(GL_TRIANGLES,mesh->getNumberIndices(),GL_UNSIGNED_INT,NULL);

            if(queryId >= 0)
                glEndQuery(GL_TIME_ELAPSED);

            //stopQuery();
        }

    }

    foreach(ModelNode* childNode, *node->getChilds())
    {
        recursiveDraw(childNode);
    }

    mvpStack.pop();
    nodeTransStack.pop();
    nodeTransList.pop_back();

    /*
    if(node == rootNode)
    {
        showErrors = false;
    }
    */
}

/**
 * @brief OGLwindow::setNewSettings If something for drawing in application changed then this method will
 *  negate all settings and create new settings.
 * @return Return false if we canno't use new settings.
 */
bool OGLwindow::setNewSettings()
{
    resetUniformTimers();

    removeSettings();

    ModelNode* node = infoM->getActiveProject()->getModel()->getRootNode();

    if(node == NULL)
    {
        return false;
    }

    rootNode = node;

    //QList<MetaShaderProg *> outPrograms = infoM->getActiveProject()->getModel()->getAttachedPrograms();
    QList<MetaShaderProg *> outPrograms = infoM->getActiveProject()->getPrograms();

    if(outPrograms.isEmpty())
        return false;

    foreach(MetaShaderProg* prog, outPrograms)
    {
        this->programs.insert(prog->getName(), prog->copy());
    }

    if(!createNewBuffers())
        return false;

    if(!loadAllShaders(this->programs.values()))
    {
        return false;
    }

    createUniformTimers();

    qDeleteAll(textures);
    textures.clear();

    if(!createTextures())
        return false;

    showErrors = true;
    availableSettings = false;

    canRender = true;
    return true;
}

/**
 * @brief OGLwindow::removeSettings Clear old drawing settings.
 */
void OGLwindow::removeSettings()
{
    glDeleteVertexArrays(1,&vao);
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    foreach(QGLBuffer* buffer, vertexBuffers)
    {
        buffer->destroy();
    }

    foreach(QGLBuffer* buffer, normalBuffers)
    {
        buffer->destroy();
    }

    foreach(QGLBuffer* buffer, colorBuffers)
    {
        buffer->destroy();
    }

    foreach(QGLBuffer* buffer, texCoordBuffers)
    {
        buffer->destroy();
    }

    foreach(QGLBuffer* buffer, indexBuffers)
    {
        buffer->destroy();
    }

    qDeleteAll(vertexBuffers);
    qDeleteAll(normalBuffers);
    qDeleteAll(colorBuffers);
    qDeleteAll(texCoordBuffers);
    qDeleteAll(indexBuffers);
    vertexBuffers.clear();
    normalBuffers.clear();
    colorBuffers.clear();
    texCoordBuffers.clear();
    indexBuffers.clear();

    qDeleteAll(programs);
    programs.clear();

    qDeleteAll(textures);
    textures.clear();

    removeQueries();
    qDeleteAll(profiles);
    profiles.clear();

    // remove timers
    qDeleteAll(uniformTimers);
    uniformTimers.clear();
    timeUniforms.clear();
    pressedUniforms.clear();
}

/**
 * @brief OGLwindow::createQuery Create new OpenGL query and save it to time query measure object.
 * Emit signals about creating new objects.
 * @param progName Shader program name what we want create query.
 */
GLint OGLwindow::createQuery(const QString progName)
{
    TimeQueryStorage* query;

    if(!profiles.contains(progName))
    {
        query = new TimeQueryStorage(progName);
        this->profiles.insert(progName, query);
        emit queryCreated(progName);
    }
    else
        query = profiles.value(progName);

    if(!query->isUsable())
        return -1;

    GLuint timeQuery;
    //for drawing time measurement
    glGenQueries(1, &timeQuery);

    //qDebug() << "Query created" << timeQuery << "for" << progName;
    query->pushQuery(timeQuery);

    return timeQuery;

    //glBeginQuery(GL_TIME_ELAPSED,timeQuery);
}

/**
 * @brief OGLwindow::testQuery Test time query measure objects if they get their last drawing values.
 * If not they set flag, for not to create new one.
 */
void OGLwindow::testQuery()
{
    foreach(TimeQueryStorage* s, profiles.values())
    {
        // test if all queries from last draw was used
        // if yes set time to final time and zero time value
        s->testUsage();
    }
}

/**
 * @brief OGLwindow::getQueryResults Get result of all queries and if result is available set it back to time query object.
 */
void OGLwindow::getQueryResults()
{
    foreach(TimeQueryStorage* q, profiles.values())
    {
        double elapsed = q->getTime();

        while(!q->isQueryEmpty())
        {
            GLint ret = GL_FALSE;
            glGetQueryObjectiv(q->topQuery(), GL_QUERY_RESULT_AVAILABLE, &ret);

            if(ret)
            {
                GLint64 qResult = 0;

                glGetQueryObjecti64v(q->topQuery(), GL_QUERY_RESULT, &qResult);

                elapsed += qResult;

                GLuint id = q->popQuery();
                glDeleteQueries(1, &id);
            }
            else
                break;

        }

        q->setTime(elapsed);
    }
}

/**
 * @brief OGLwindow::removeQueries Remove all time query measure objects and emit signals about this.
 */
void OGLwindow::removeQueries()
{
    foreach(TimeQueryStorage* s, profiles)
    {
        const int size = s->getQuerySize();
        GLuint *id = new GLuint[size];
        int counter = 0;

        while(!s->isQueryEmpty())
        {
            id[counter] = s->popQuery();
            ++counter;
        }

        glDeleteQueries(counter, id);

        emit queryDestroyed(s->getName());
        delete id;
    }
}

/**
 * @brief OGLwindow::setMVP
 */
void OGLwindow::setMVP(QString programName, QMatrix4x4 modelMatrix)
{
    QGLShaderProgram* glprog = shaders.value(programName);
    glprog->bind();

    int loc;

    loc = glprog->uniformLocation(MVP_CHAR);

    if(loc != -1)
        glprog->setUniformValue(loc,mvpStack.top());

    /*
    loc = glprog->uniformLocation(PROJECTION_CHAR);

    if(loc != -1)
        glprog->setUniformValue(loc,projection);

    loc = glprog->uniformLocation(VIEW_CHAR);

    if(loc != -1)
        glprog->setUniformValue(loc,view);

    */
    loc = glprog->uniformLocation(MODEL_CHAR);

    if(loc != -1)
        glprog->setUniformValue(loc,modelMatrix);

    /*
    foreach(UniformVariable* u, unif)
    {

        foreach(QString var, variables)
        {
            int loc = glprog->uniformLocation(var);

            if(loc < 0)
            {
                if(printWarning)
                    log.addUniformError("Shader variable '" + var + "' didn't exists.");
            }
            else
            {
                attachShaderUniform(u,loc,glprog);
            }
        }
    }
    */
}

/**
 * @brief OGLwindow::testProjection Test for uniform variable projection matrix. If not found standard projection matrix will be used.
 */
void OGLwindow::testProjection()
{
    UniformVariable* var = infoM->getActiveProject()->getUniformVariable("projection");

    mvpStack.clear();

    if(var == NULL)
        mvpStack.push(projection);
    else
    {
        QMatrix4x4 p = var->getValueMat4x4();
        mvpStack.push(p);
    }

    mvpStack.push(mvpStack.top() * view);
}

/**
 * @brief OGLwindow::testView Test uniform variable view matrix. If not exists use default view matrix.
 */
void OGLwindow::testView()
{
    UniformVariable* var = infoM->getActiveProject()->getUniformVariable("view");

    if(var == NULL)
        return;
    else
    {
        QMatrix4x4 v = calculateMultiplyMat4x4(*var, showErrors);
        mvpStack.pop();
        mvpStack.push(mvpStack.top() * v);
    }
}

/**
 * @brief OGLwindow::calculateMultiplyMat4x4 Calculate multiply matrices MAT4x4.
 * @param variable This given uniform variable is in multiply mode.
 * @param showErrors If print errors about bad matrices.
 * @return Result of multiplication.
 */
QMatrix4x4 OGLwindow::calculateMultiplyMat4x4(const UniformVariable &variable, bool showErrors)
{
    if(!variable.isMultiplyMode())
    {
        if(showErrors)
            log.addUniformError(tr("Error uniform variable %1 is not in multiply mode").arg(variable.getName()));

        return QMatrix4x4();
    }

    QString name = variable.getName();
    QMatrix4x4 result;
    MetaProject* proj = infoM->getActiveProject();

    QList<QVariant> list = variable.getValues();

    foreach(QVariant var, list)
    {
        UniformVariable* u = proj->getUniformVariable(var.toString());

        if(!testMultiplyVar(name,var.toString(),u,showErrors))
        {
            return QMatrix4x4();
        }

        if(u->getUniformSize() != UniformTypes::MAT4)
        {
            if(showErrors)
                log.addUniformError(tr("When computing multiply mode uniform variable %1, %2 is not Matrix 4x4!")
                                    .arg(name, var.toString()));

            return QMatrix4x4();
        }

        result *= u->getValueMat4x4();
    }

    return result;
}

/**
 * @brief OGLwindow::calculateMultiplyMat3x3 Calculate multiply matrices MAT3x3.
 * @param variable This given uniform variable is in multiply mode.
 * @param showErrors If print errors about bad matrices.
 * @return Result of multiplication.
 */
QMatrix3x3 OGLwindow::calculateMultiplyMat3x3(const UniformVariable &variable, bool showErrors)
{
    if(!variable.isMultiplyMode())
    {
        if(showErrors)
            log.addUniformError(tr("Error uniform variable %1 is not in multiply mode").arg(variable.getName()));

        return QMatrix3x3();
    }

    QString name = variable.getName();
    QMatrix3x3 result;
    MetaProject* proj = infoM->getActiveProject();

    QList<QVariant> list = variable.getValues();

    foreach(QVariant var, list)
    {
        UniformVariable* u = proj->getUniformVariable(var.toString());

        if(!testMultiplyVar(name,var.toString(),u,showErrors))
        {
            return QMatrix3x3();
        }

        if(u->getUniformSize() != UniformTypes::MAT3)
        {
            if(showErrors)
                log.addUniformError(tr("When computing multiply mode uniform variable %1, %2 is not Matrix 3x3!")
                                    .arg(name, var.toString()));

            return QMatrix3x3();
        }

        result = result * u->getValueMat3x3();
    }

    return result;
}

/**
 * @brief OGLwindow::calculateMultiplyMat2x2 Calculate multiply matrices MAT4x4.
 * @param variable This given uniform variable is in multiply mode.
 * @param showErrors If print errors about bad matrices.
 * @return Result of multiplication.
 */
QMatrix2x2 OGLwindow::calculateMultiplyMat2x2(const UniformVariable &variable, bool showErrors)
{
    if(!variable.isMultiplyMode())
    {
        if(showErrors)
            log.addUniformError(tr("Error uniform variable %1 is not in multiply mode").arg(variable.getName()));

        return QMatrix2x2();
    }

    QString name = variable.getName();
    QMatrix2x2 result;
    MetaProject* proj = infoM->getActiveProject();

    QList<QVariant> list = variable.getValues();

    foreach(QVariant var, list)
    {
        UniformVariable* u = proj->getUniformVariable(var.toString());

        if(!testMultiplyVar(name,var.toString(),u,showErrors))
        {
            return QMatrix2x2();
        }

        if(u->getUniformSize() != UniformTypes::MAT2)
        {
            if(showErrors)
                log.addUniformError(tr("When computing multiply mode uniform variable %1, %2 is not Matrix 2x2!")
                                    .arg(name, var.toString()));

            return QMatrix2x2();
        }

        result = result * u->getValueMat2x2();
    }

    return result;
}

/**
 * @brief OGLwindow::calculateMultiplyVec4 Calculate multiply vector VEC4.
 * @param variable This given uniform variable is in multiply mode.
 * @param showErrors If print errors about bad matrices.
 * @return Result of multiplication.
 */
QVector4D OGLwindow::calculateMultiplyVec4(const UniformVariable &variable, bool showErrors)
{
    if(!variable.isMultiplyMode())
    {
        if(showErrors)
            log.addUniformError(tr("Error uniform variable %1 is not in multiply mode").arg(variable.getName()));

        return QVector4D();
    }

    QString name = variable.getName();
    QVector4D result;
    MetaProject* proj = infoM->getActiveProject();

    QList<QVariant> list = variable.getValues();

    foreach(QVariant var, list)
    {
        UniformVariable* u = proj->getUniformVariable(var.toString());

        if(!testMultiplyVar(name,var.toString(),u,showErrors))
        {
            return QVector4D();
        }

        if(u->getUniformSize() != UniformTypes::VEC4)
        {
            if(showErrors)
                log.addUniformError(tr("When computing multiply mode uniform variable %1, %2 is not Matrix 4x4!")
                                    .arg(name, var.toString()));

            return QVector4D();
        }

        result *= u->getValueVec4D();
    }

    return result;
}

/**
 * @brief OGLwindow::calculateMultiplyVec3 Calculate multiply vector VEC3.
 * @param variable This given uniform variable is in multiply mode.
 * @param showErrors If print errors about bad matrices.
 * @return Result of multiplication.
 */
QVector3D OGLwindow::calculateMultiplyVec3(const UniformVariable &variable, bool showErrors)
{
    if(!variable.isMultiplyMode())
    {
        if(showErrors)
            log.addUniformError(tr("Error uniform variable %1 is not in multiply mode").arg(variable.getName()));

        return QVector3D();
    }

    QString name = variable.getName();
    QVector3D result;
    MetaProject* proj = infoM->getActiveProject();

    QList<QVariant> list = variable.getValues();

    foreach(QVariant var, list)
    {
        UniformVariable* u = proj->getUniformVariable(var.toString());

        if(!testMultiplyVar(name,var.toString(),u,showErrors))
        {
            return QVector3D();
        }

        if(u->getUniformSize() != UniformTypes::VEC3)
        {
            if(showErrors)
                log.addUniformError(tr("When computing multiply mode uniform variable %1, %2 is not Matrix 4x4!")
                                    .arg(name, var.toString()));

            return QVector3D();
        }

        result *= u->getValueVec3D();
    }

    return result;
}

/**
 * @brief OGLwindow::calculateMultiplyVec2 Calculate multiply vector VEC2.
 * @param variable This given uniform variable is in multiply mode.
 * @param showErrors If print errors about bad matrices.
 * @return Result of multiplication.
 */
QVector2D OGLwindow::calculateMultiplyVec2(const UniformVariable &variable, bool showErrors)
{
    if(!variable.isMultiplyMode())
    {
        if(showErrors)
            log.addUniformError(tr("Error uniform variable %1 is not in multiply mode").arg(variable.getName()));

        return QVector2D();
    }

    QString name = variable.getName();
    QVector2D result;
    MetaProject* proj = infoM->getActiveProject();

    QList<QVariant> list = variable.getValues();

    foreach(QVariant var, list)
    {
        UniformVariable* u = proj->getUniformVariable(var.toString());

        if(!testMultiplyVar(name,var.toString(),u,showErrors))
        {
            return QVector2D();
        }

        if(u->getUniformSize() != UniformTypes::VEC2)
        {
            if(showErrors)
                log.addUniformError(tr("When computing multiply mode uniform variable %1, %2 is not Matrix 4x4!")
                                    .arg(name, var.toString()));

            return QVector2D();
        }

        result *= u->getValueVec2D();
    }

    return result;
}

/**
 * @brief OGLwindow::convertToGLEnum Convert SettingStorage blend function enums to GLenum format.
 * @param func SettingStorage enum for convertion.
 * @return GLenum with the same meaning.
 */
GLenum OGLwindow::convertToGLEnum(SettingsStorage::BLEND_FUNCTION func) const
{
    GLenum ret;

    switch(func)
    {
    case SettingsStorage::ZERO:
        ret = GL_ZERO;
    case SettingsStorage::ONE:
        ret = GL_ONE;
    case SettingsStorage::DST_COLOR:
        ret = GL_DST_COLOR;
    case SettingsStorage::SRC_COLOR:
        ret = GL_SRC_COLOR;
    case SettingsStorage::ONE_MINUS_DST_COLOR:
        ret = GL_ONE_MINUS_DST_COLOR;
    case SettingsStorage::ONE_MINUS_SRC_COLOR:
        ret = GL_ONE_MINUS_SRC_COLOR;
    case SettingsStorage::SRC_ALPHA:
        ret = GL_SRC_ALPHA;
    case SettingsStorage::ONE_MINUS_SRC_ALPHA:
        ret = GL_ONE_MINUS_SRC_ALPHA;
    case SettingsStorage::DST_ALPHA:
        ret = GL_DST_ALPHA;
    case SettingsStorage::ONE_MINUS_DST_ALPHA:
        ret = GL_ONE_MINUS_DST_ALPHA;
    case SettingsStorage::SRC_ALPHA_SATURATE:
        ret = GL_SRC_ALPHA_SATURATE;
    }

    return ret;
}

/**
 * @brief OGLwindow::testMultiplyVar Test uniform variable if is multiply mode and if exists.
 * @param name Name of the original multiply variable.
 * @param innerVar Name of the inner variable of multiply variable.
 * @param var Inner variable.
 * @param showErrors If warning will be printed.
 * @return True if test is ok, false if some error ocurse.
 */
bool OGLwindow::testMultiplyVar(QString name, QString innerVar, UniformVariable *var, bool showErrors)
{
    if(!var)
    {
        if(showErrors)
            log.addUniformError(tr("When computing multiply mode uniform variable %1, %2 variable do not exists!")
                                .arg(name,innerVar));

        return false;
    }

    if(var->isMultiplyMode())
    {
        if(showErrors)
            log.addUniformError(tr("When computing multiply mode uniform variable %1, %2 variable canno't be multiply variable!")
                                .arg(name,innerVar));

        return false;
    }

    return true;
}

/**
 * @brief OGLwindow::checkError Check if OpenGL reporting any error.
 * @return Return error string if no error was created return empty string.
 */
QString OGLwindow::checkError()
{
    GLenum e = glGetError();

    switch(e)
    {
    case GL_NO_ERROR:
        return "";
    case GL_INVALID_ENUM:
        return "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
    case GL_INVALID_VALUE:
        return "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";
    case GL_INVALID_OPERATION:
        return "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";
    case GL_OUT_OF_MEMORY:
        return "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
    case GL_STACK_UNDERFLOW:
        return "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
    case GL_STACK_OVERFLOW:
        return "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
    default:
        return "";
    }
}

/**
 * @brief OGLwindow::initializeGL Initialize OpenGL settings.
 */
void OGLwindow::initializeGL()
{
    QGLFormat glFormat = QGLWidget::format();

    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    makeCurrent();

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
        qWarning() << "Error could not initialize extensions";

    if(!GLEW_VERSION_3_3)
        qWarning() << "OpenGL version 3.3 is not supported";

    if(!GLEW_ARB_vertex_array_object)
        qWarning() << "Vertex array extension isn't here";

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // We need us some vertex data. Start simple with a triangle ;-)
    float points[] = { -0.5f, -0.5f, 0.0f, 1.0f,
                        0.5f, -0.5f, 0.0f, 1.0f,
                        0.0f,  0.5f, 0.0f, 1.0f };

    vertexBuffers.append(new QGLBuffer(QGLBuffer::VertexBuffer));

    vertexBuffers.first()->create();
    vertexBuffers.first()->setUsagePattern( QGLBuffer::StaticDraw );

    if ( !vertexBuffers.first()->bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    vertexBuffers.first()->allocate( points, 3 * 4 * sizeof( float ) );

    //enable OpenGL functions
    glEnable(GL_DEPTH_TEST);
}

/**
 * @brief OGLwindow::resizeGL Resize OpenGL window and set new viewport.
 * @param w Horizontal size of this window.
 * @param h Vertival size of this window.
 */
void OGLwindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, qMax(h, 1));
}

/**
 * @brief OGLwindow::paintGL Drawing method, it is called when updateGL is called.
 */
void OGLwindow::paintGL()
{
    // clear color buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!canRender)
        return;

    if(availableSettings)
    {
        if(!setNewSettings())
        {
            canRender = false;
            return;
        }
    }

    testQuery();

    QMatrix4x4 rot;
    rot.setToIdentity();
    rot.rotate(roty,1.0f,0.0f);
    rot.rotate(rotx,0.0f,1.0f);

    QMatrix4x4 zoom;
    zoom.setToIdentity();
    zoom.translate(0.0,0.0,zoomZ);

    view.setToIdentity();
    view *= zoom * rot;

    nodeTransList.clear();
    nodeTransStack.clear();
    //mvpStack.push_back(mvpStack.top() * view);
    testProjection();
    testView();
    //m_shader->setUniformValue(mvp_loc,mvpStack.top());

    //qDebug() << "Drawing recursively";
    recursiveDraw(rootNode);

    mvpStack.pop();

    QString error = checkError();

    if((!error.isEmpty()) && showErrors)
        log.addToLog("OpenGL ERROR:" + error + '\n');

    showErrors = false;

    getQueryResults();
}

/** SLOTS **/

/**
 * @brief OGLwindow::rotTimeout Timeout for changing mvp matrix settings.
 */
void OGLwindow::rotTimeout()
{
    if(rotx > 360)
        rotx = 0;

    if(roty > 360)
        roty = 0;

    if(buttonPressedField[XP])
        rotx++;
    else if(buttonPressedField[XM])
        rotx--;

    if(buttonPressedField[YP])
        roty--;
    else if(buttonPressedField[YM])
        roty++;

    if(buttonPressedField[ZP])
        zoomZ++;
    else if(buttonPressedField[ZM])
        zoomZ--;

    updateGL();
}

/**
 * @brief OGLwindow::incUnifTimeTimers Increment uniform special variable $Time{inc,time,default=0,max=0}.
 * If timer will timeout interval call this method.
 * @param id Id of the setted timer. It is the interval which the timer waiting.
 */
void OGLwindow::incUnifTimeTimers(long id)
{
    if(!canRender || isDrawPaused)
        return;

    foreach(UniformVariable* u, timeUniforms)
    {
        u->incrementTimeTimers(id);
    }
}

/**
 * @brief OGLwindow::incUnifActionPressedTimers Increment uniform special variable
 * $ActionPressed{BtnId,time,true value,false value,default=0}.
 * If timer will timeout interval id call this method.
 * If user pressing button (BtnId) then increment base value with true value, if not increment base value with false value.
 * @param id Id of the setted timer. It is the interval which the timer waiting.
 */
void OGLwindow::incUnifActionPressedTimers(long id)
{
    if(!canRender || isDrawPaused)
        return;

    foreach(UniformVariable* u, pressedUniforms)
    {
        u->incrementActionPressedTimers(id, buttonPressedField);
    }
}

/**
 * @brief OGLwindow::buttonPressed When button under OpenGL window will be pressed this method is called.
 * Identify which button was clicked and toggle all Action variables and remember pressed button for ActionPressed variable.
 */
void OGLwindow::buttonPressed()
{
    QObject* button = sender();
    QString buttonName = button->objectName();
    QString num;

    if(!infoM->isActiveProject())
        return;

    // first test for rotation and zoom buttons
    if(buttonName.at(buttonName.size()-2).isDigit())
    {
        num = buttonName.right(2);
    }
    else // after this test for universal buttons
    {
        num = buttonName.right(1);
    }

    bool isOk = false;
    int btnId = num.toInt(&isOk);

    if(!isOk)
        return;

    buttonPressedField[btnId] = true;
    toggleAll(btnId);
}

/**
 * @brief OGLwindow::buttonReleased When button under OpenGL window will be released this method is called.
 * Identify which button was released and rember this.
 */
void OGLwindow::buttonReleased()
{
    QObject* button = sender();
    QString buttonName = button->objectName();
    QString num;

    // first test for rotation and zoom buttons
    if(buttonName.at(buttonName.size()-2).isDigit())
    {
        num = buttonName.right(2);
    }
    else // after this test for universal buttons
    {
        num = buttonName.right(1);
    }

    bool isOk = false;
    int btnId = num.toInt(&isOk);

    if(!isOk)
        return;

    buttonPressedField[btnId] = false;
}

/**
 * @brief OGLwindow::setOpenGLSettings Set project options to OpenGL.
 */
void OGLwindow::setOpenGLSettings()
{
    MetaProject* actProj = infoM->getActiveProject();

    if(actProj == NULL)
        return;

    SettingsStorage* settings = actProj->getSettings();

    // clear color
    const float *colors = settings->getBackgroundColors();
    glClearColor(colors[0], colors[1], colors[2], colors[3]);

    // blending
    if(settings->isBlending())
    {
        glEnable(GL_BLEND);
        glDepthMask(0);

        GLenum sFactor = convertToGLEnum(settings->getBlendSFactor());
        GLenum dFactor = convertToGLEnum(settings->getBlendDFactor());

        glBlendFunc(sFactor, dFactor);
    }
    else {
        glDisable(GL_BLEND);
        glDepthMask(1);
    }

    // depth test
    if(settings->isDepthTest())
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}


/**
 * @brief OGLwindow::runShaders Set new shaders for drawing.
 */
void OGLwindow::runShaders()
{
    qDebug() << "Loading new shaders to OpenGL window";

    if(!isRenderValid())
    {
        canRender = false;
        return;
    }

    //setNewSettings();
    canRender = true;
    availableSettings = true;
    updateGL();
}

/**
 * @brief OGLwindow::loadNewModel Load new 3D model for drawing.
 */
void OGLwindow::loadNewModel()
{
    qDebug() << "Loading new model to OpenGL window";

    if(!isRenderValid())
    {
        canRender = false;
        return;
    }

    //setNewSettings();

    canRender = true;
    availableSettings = true;
    updateGL();
}

/**
 * @brief OGLwindow::reloadShaderPrograms Load new settings for shader programs.
 */
void OGLwindow::reloadShaderPrograms()
{
    qDebug() << "Refresh settings from shader programs.";

    if(!isRenderValid())
        return;

    //setNewSettings();

    canRender = true;
    availableSettings = true;
    updateGL();
}

/**
 * @brief OGLwindow::newTextures Textures for drawing was changed, we need to create new one for OpenGL.
 */
void OGLwindow::newTextures()
{
    qDebug() << "Create new textures for OpenGL.";

    if(!isRenderValid())
        return;

    canRender = true;
    availableSettings = true;
    updateGL();
}

/**
 * @brief OGLwindow::newUniformValues New uniform variables was setted, load new settings.
 */
void OGLwindow::newUniformValues()
{
    qDebug() << "Create new uniform values";

    if(!isRenderValid())
        return;

    canRender = true;
    availableSettings = true;
    updateGL();
}

/**
 * @brief OGLwindow::pauseDrawing Pause drawing for few moments.
 * @param pause True if we want pause drawing, false otherwise.
 */
void OGLwindow::pauseDrawing(bool pause)
{
    isDrawPaused = pause;
}

/**
 * @brief OGLwindow::rotateObjectBtnPressed Reaction to rotate buttons from MainWindow.
 * Mark where we want rotate, compute later.
 */
/*
void OGLwindow::rotateObjectBtnPressed()
{
    QString name = sender()->objectName();

    qDebug() << name;

    if(name.startsWith("up"))
        rotate[Y+] = true;
    else if(name.startsWith("down"))
        rotate[Y-] = true;
    else if(name.startsWith("right"))
        rotate[X+] = true;
    else if(name.startsWith("left"))
        rotate[X-] = true;
}
*/

/**
 * @brief OGLwindow::rotateObjectBtnReleased Reaction to release rotate button from MainWindow.
 * Mark where we do not want rotate, compute later.
 */
/*
void OGLwindow::rotateObjectBtnReleased()
{
    QString name = sender()->objectName();

    qDebug() << name;

    if(name.startsWith("up"))
        rotate[Y+] = false;
    else if(name.startsWith("down"))
        rotate[Y-] = false;
    else if(name.startsWith("right"))
        rotate[X+] = false;
    else if(name.startsWith("left"))
        rotate[X-] = false;
}
*/

/**
 * @brief OGLwindow::zoomObjectBtnPressed Reaction to press zoom button from MainWindow.
 * Mark if we want zoom in or out, compute later.
 */
/*
void OGLwindow::zoomObjectBtnPressed()
{
    QString name = sender()->objectName();

    qDebug() << name;

    if(name.startsWith("in"))
        zoom[Z-] = true;
    else if(name.startsWith("out"))
        zoom[Z+] = true;
}
*/

/**
 * @brief OGLwindow::zoomObjectBtnReleased Reaction to release zoom button from MainWindow.
 * Mark if we do not want zoom in or out, compute later.
 */
/*
void OGLwindow::zoomObjectBtnReleased()
{
    QString name = sender()->objectName();

    qDebug() << name;

    if(name.startsWith("in"))
        zoom[Z-] = false;
    else if(name.startsWith("out"))
        zoom[Z+] = false;
}
*/

