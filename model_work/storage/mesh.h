#ifndef MESH_H
#define MESH_H

#include <QList>
#include <QDebug>

/**
 * @brief The Mesh class contains all important data about mesh.
 */
class Mesh
{
public:
    explicit Mesh(unsigned int numVertex, float* vertices,
                  unsigned int numNormals, float* normals,
                  QList<unsigned int> numColors, QList<float*> colors,
                  QList<unsigned int> numTexCoords, QList<float*> texCoords,
                  unsigned int numIndices, unsigned int* indices);
    ~Mesh();

    static void setIndexCounter(unsigned long startIndex);

    // get methods
    unsigned int getIndex();

    float* getVertices();
    float* getNormals();
    float* getColors(int id = 0);
    float* getTexCoords(int id = 0);
    unsigned int *getIndices();

    unsigned int getNumberVertices();
    unsigned int getNumberColors(int id = 0);
    unsigned int getNumberTexCoords(int id =0);
    unsigned int getNumberNormals();
    unsigned int getNumberIndices();

    size_t getSizeVertices();
    size_t getSizeColors(int id = 0);
    size_t getSizeTexCoords(int id =0);
    size_t getSizeNormals();
    size_t getSizeIndices();

    unsigned int getVertexBufferIndex();
    unsigned int getColorBufferIndex(int id = 0);
    unsigned int getTextCoordBufferIndex(int id = 0);
    unsigned int getNormalBufferIndex();
    unsigned int getIndexBufferIndex();

    // set methods
    void setVertexBufferIndex(unsigned int id);
    void addColorBufferIndex(unsigned int id);
    void addTexCoordBufferIndex(unsigned int id);
    void setNormalBufferIndex(unsigned int id);
    void setIndexBufferIndex(unsigned int id);

    // vertex array methods
    unsigned int getVertexArrayBuffer();
    void setVertexArrayBuffer(unsigned int id);

    // has methods
    bool hasVertices();
    bool hasNormals();
    bool hasColors();
    bool hasTexCoords();
    bool hasIndices();

    // get buffers count
    unsigned int getColorBuffersCount();
    unsigned int getTexCoordBuffersCount();

private:
    static unsigned long counter;
    unsigned int index;
    unsigned int numVert;
    float* vertices;
    unsigned int numNormals;
    float* normals;
    QList<unsigned int> numColors;
    QList<float*> colors;
    unsigned int numIndices;
    unsigned int* indices;
    QList<unsigned int> numTexCoords;
    QList<float*> texCoords;

    unsigned int vBuffer;
    unsigned int nBuffer;
    QList<unsigned int> cBuffer;
    QList<unsigned int> tBuffer;
    unsigned int iBuffer;
    unsigned int vaBuffer;
};

#endif // MESH_H
