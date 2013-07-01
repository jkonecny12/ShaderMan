#include "mesh.h"
#include <QtAlgorithms>

//static member of class
unsigned long Mesh::counter = 0;

/**
 * @brief Mesh::Mesh Create mesh object.
 * @param numVertex How many vertices is in this object.
 * @param vertices Vertices array.
 * @param numNormals How many normals is in this object.
 * @param normals Normals array.
 * @param numColors How many colors is in this object.
 * @param colors Colors array.
 * @param numTexCoords How many texture coordinates is in this object.
 * @param texCoords Texture coordinates array.
 * @param numIndices How many indices is in this object.
 * @param indices Indices array.
 */
Mesh::Mesh(unsigned int numVertex, float *vertices,
           unsigned int numNormals, float *normals,
           QList<unsigned int> numColors, QList<float *> colors,
           QList<unsigned int> numTexCoords, QList<float *> texCoords,
           unsigned int numIndices, unsigned int *indices)
{
    this->numVert = numVertex;
    this->numNormals = numNormals;
    this->numColors = numColors;
    this->numTexCoords = numTexCoords;
    this->numIndices = numIndices;
    this->vertices = vertices;
    this->normals = normals;
    this->colors = colors;
    this->texCoords = texCoords;
    this->indices = indices;
    this->vBuffer = 0;
    this->nBuffer = 0;
    this->iBuffer = 0;

    index = counter;
    counter++;

    qDebug() << "Created mesh with index: " << index;
}

/**
 * @brief Mesh::~Mesh Destroy this object and free memory.
 */
Mesh::~Mesh()
{
    delete this->vertices;
    delete this->normals;
    //delete this->colors;
    delete this->indices;

    qDeleteAll(colors);
    qDeleteAll(texCoords);
}

/**
 * @brief Mesh::setIndexCounter Set id counter for identified new created meshes
 * @param startIndex New starting index for meshes
 */
void Mesh::setIndexCounter(unsigned long startIndex)
{
    counter = startIndex;
}

/**
 * @brief Mesh::getIndex Return index of this mesh, generated automaticaly when object created
 * @return Mesh index
 */
unsigned int Mesh::getIndex()
{
    return index;
}

/* Get data methods */

/**
 * @brief Mesh::getVertices Return values of vertices for this mesh
 * @return Vertices in this mesh as array of floats
 */
float *Mesh::getVertices()
{
    return vertices;
}

/**
 * @brief Mesh::getNormals Return values of normals for this mesh
 * @return Normals in this mesh as array of floats
 */
float *Mesh::getNormals()
{
    return normals;
}

/**
 * @brief Mesh::getColors Return values of colors for this mesh.
 * @param id Identify what color we want.
 * @return Colors in this mesh as array of floats.
 */
float *Mesh::getColors(int id)
{
    return colors.value(id);
}

/**
 * @brief Mesh::getTexCoords Return values of texture coordinate for this mesh.
 * @param id Identify what texture coordinate we want.
 * @return Texture coordinate int this mesh as array of floats.
 */
float *Mesh::getTexCoords(int id)
{
    return texCoords.value(id);
}

/**
 * @brief Mesh::getIndices Return values of indices for this mesh
 * @return Indices in this mesh as array of unsigned ints
 */
unsigned int *Mesh::getIndices()
{
    return indices;
}

/* Get counts methods */

/**
 * @brief Mesh::getNumberVertices Return number of vertices (floats) in this mesh
 * @return Number of vertices (floats in array)
 */
unsigned int Mesh::getNumberVertices()
{
    return numVert;
}

/**
 * @brief Mesh::getNumberColors Return number of colors (floats) in this mesh.
 * @param id Identify what color we want.
 * @return Number of colors (floats in array).
 */
unsigned int Mesh::getNumberColors(int id)
{
    if(numColors.isEmpty())
        return 0;

    return numColors.value(id);
}

/**
 * @brief Mesh::getNumberTexCoords Return number of texture coordinates (floats) in this mesh.
 * @param id Identify what texture coordinate we want.
 * @return Number of texture coordinates (floats in array).
 */
unsigned int Mesh::getNumberTexCoords(int id)
{
    if(numTexCoords.isEmpty())
        return 0;

    return numTexCoords.value(id);
}

/**
 * @brief Mesh::getNumberNormals Return number of normals (floats) in this mesh
 * @return Number of normals (floats in array)
 */
unsigned int Mesh::getNumberNormals()
{
    return numNormals;
}

/**
 * @brief Mesh::getNumberIndices Return number of indices (unsigned ints) in this mesh
 * @return Number of indices (unsigned ints in array)
 */
unsigned int Mesh::getNumberIndices()
{
    return numIndices;
}

/* Get size methods */

/**
 * @brief Mesh::getSizeVertices Return size of vertices array in bytes
 * @return Size in bytes
 */
size_t Mesh::getSizeVertices()
{
    return numVert * sizeof(float);
}

/**
 * @brief Mesh::getSizeColors Return size of colors array in bytes.
 * @param id Identify what color buffer we want.
 * @return Size in bytes.
 */
size_t Mesh::getSizeColors(int id)
{
    if(numColors.isEmpty())
        return 0;

    return numColors.value(id) * sizeof(float);
}

/**
 * @brief Mesh::getSizeTexCoords Return size of texture coordinate array in bytes.
 * @param id Identify what texture coordinate buffer we want.
 * @return Size in bytes.
 */
size_t Mesh::getSizeTexCoords(int id)
{
    if(numTexCoords.isEmpty())
        return 0;

    return numTexCoords.value(id) * sizeof(float);
}

/**
 * @brief Mesh::getSizeNormals Return size of normals array in bytes.
 * @return Size in bytes.
 */
size_t Mesh::getSizeNormals()
{
    return numNormals * sizeof(float);
}

/**
 * @brief Mesh::getSizeIndices Return size of indices array as bytes
 * @return Size in bytes
 */
size_t Mesh::getSizeIndices()
{
    return numIndices * sizeof(unsigned int);
}

/* Get buffer index methods */

/**
 * @brief Mesh::getVertexBufferIndex Return index of OpenGL vertex buffer
 * @return Index of buffer
 */
unsigned int Mesh::getVertexBufferIndex()
{
    return vBuffer;
}

/**
 * @brief Mesh::getColorBufferIndex Return index of OpenGL color buffer.
 * @param id Identify what color buffer we want.
 * @return Index of buffer.
 */
unsigned int Mesh::getColorBufferIndex(int id)
{
    //return cBuffer;
    if(cBuffer.isEmpty())
        return 0;

    return cBuffer.value(id);
}

/**
 * @brief Mesh::getTextCoordBufferIndex Return index of OpenGL texture coordinate buffer.
 * @param id Identify what texture coordinate we want.
 * @return Index of buffer.
 */
unsigned int Mesh::getTextCoordBufferIndex(int id)
{
    if(tBuffer.isEmpty())
        return 0;

    return tBuffer.value(id);
}

/**
 * @brief Mesh::getNormalBufferIndex Return index of OpenGL normal buffer
 * @return Index of buffer
 */
unsigned int Mesh::getNormalBufferIndex()
{
    return nBuffer;
}

/**
 * @brief Mesh::getIndexBufferIndex Return index of OpenGL element buffer
 * @return Index of buffer
 */
unsigned int Mesh::getIndexBufferIndex()
{
    return iBuffer;
}

/* Set buffer index methods */

/**
 * @brief Mesh::setVertexBufferIndex Set index of OpenGL vertex buffer for this mesh
 * @param id Index of this buffer
 */
void Mesh::setVertexBufferIndex(unsigned int id)
{
    this->vBuffer = id;
}

/**
 * @brief Mesh::setColorBufferIndex Set index of OpenGL color buffer for this mesh
 * @param id Index of this buffer
 */
void Mesh::addColorBufferIndex(unsigned int id)
{
    //this->cBuffer = id;
    this->cBuffer.append(id);
}

/**
 * @brief Mesh::addTexCoordBufferIndex Set index of OpenGL texture buffer for this mesh.
 * @param id Index of this buffer.
 */
void Mesh::addTexCoordBufferIndex(unsigned int id)
{
    this->tBuffer.append(id);
}

/**
 * @brief Mesh::setNormalBufferIndex Set index of OpenGL normal buffer for this mesh
 * @param id Index of this buffer
 */
void Mesh::setNormalBufferIndex(unsigned int id)
{
    this->nBuffer = id;
}

/**
 * @brief Mesh::setIndexBufferIndex Set index of OpenGL element buffer for this mesh
 * @param id Index of this buffer
 */
void Mesh::setIndexBufferIndex(unsigned int id)
{
    this->iBuffer = id;
}

/* Work with vertex array buffers */

/**
 * @brief Mesh::getVertexArrayBuffer Return VAO object index
 * @return Index of Vertex Array Object
 */
unsigned int Mesh::getVertexArrayBuffer()
{
    return vaBuffer;
}

/**
 * @brief Mesh::setVertexArrayBuffer Set VAO object index for this mesh
 * @param id Index of Vertex Array Object
 */
void Mesh::setVertexArrayBuffer(unsigned int id)
{
    vaBuffer = id;
}

/* Question methods */

/**
 * @brief Mesh::hasVertices Has this mesh some vertices?
 * @return True if has, false otherwise
 */
bool Mesh::hasVertices()
{
    return numVert == 0 ? false : true;
}

/**
 * @brief Mesh::hasNormals Has this mesh some normals?
 * @return True if has, false otherwise
 */
bool Mesh::hasNormals()
{
    return numNormals == 0 ? false : true;
}

/**
 * @brief Mesh::hasColors Has this mesh some colors?
 * @return True if has, false otherwise
 */
bool Mesh::hasColors()
{
    return !colors.isEmpty();//colors == NULL ? false : true;
}

/**
 * @brief Mesh::hasTexCoords Has this mesh some texture coordinates?
 * @return True if has, false otherwise.
 */
bool Mesh::hasTexCoords()
{
    return !texCoords.isEmpty();
}

/**
 * @brief Mesh::hasIndices Has this mesh some indices?
 * @return True if has, false otherwise
 */
bool Mesh::hasIndices()
{
    return numIndices == 0 ? false : true;
}

/**
 * @brief Mesh::getColorBuffersCount Get number of color buffers (not OpenGL buffers) in this mesh.
 * @return Return color buffers count.
 */
unsigned int Mesh::getColorBuffersCount()
{
    return colors.size();
}

/**
 * @brief Mesh::getTexCoordBuffersCount Get number of texture coordinate buffers (not OpenGL buffers) in this mesh.
 * @return Return texture coordinate buffers count.
 */
unsigned int Mesh::getTexCoordBuffersCount()
{
    return texCoords.size();
}
