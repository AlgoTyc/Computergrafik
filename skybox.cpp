

#include "skybox.h"
#include <QImage>
#include <QVector4D>

Skybox::Skybox()
{

}
Skybox::~Skybox()
{
    glDeleteTextures(1, &m_cubeTex);
}

void Skybox::init_skybox(){

    //initialize OGLFunctions and test
    bool success =initializeOpenGLFunctions();
    Q_ASSERT(success); Q_UNUSED(success);

    // VBO data (3 foats per position)
    static const GLfloat vertexData[] = {
        - 1, 1, - 1,
        - 1,- 1, - 1,
        1, - 1, -1,
        1, 1, - 1,
        - 1, - 1, 1,
        - 1, 1, 1,
        1, - 1, 1,
        1, 1, 1,
    };
    // IBO data
    static const GLuint indicesData[] = {
        0, 1, 2, 2, 3, 0,
        4, 1, 0, 0, 5, 4,
        2, 6, 7, 7, 3, 2,
        4, 5, 7, 7, 6, 4,
        0, 3, 7, 7, 5, 0,
        1, 4, 2, 2, 4, 6,
    };

    // Create texture object
    glGenTextures(1, &m_cubeTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex);

    //Fill with pixel data
    QImage faceImg(":/right.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA,
                 faceImg.width(), faceImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 reinterpret_cast<void const*>(faceImg.constBits()));

     faceImg.load(":/left.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA,
                 faceImg.width(), faceImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 reinterpret_cast<void const*>(faceImg.constBits()));

    faceImg.load(":/back.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA,
                 faceImg.width(), faceImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 reinterpret_cast<void const*>(faceImg.constBits()));

    faceImg.load(":/front.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA,
                 faceImg.width(), faceImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 reinterpret_cast<void const*>(faceImg.constBits()));

    faceImg.load(":/top.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA,
                 faceImg.width(), faceImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 reinterpret_cast<void const*>(faceImg.constBits()));

    faceImg.load(":/bottom.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA,
                 faceImg.width(), faceImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 reinterpret_cast<void const*>(faceImg.constBits()));



    //Set filtering ( interpolation ) options
    // without these commands, _sampling will return black_
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    //Set wrap mode to "repeat"
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Create VBO and binds it
    glGenBuffers(1, &m_vbo_skybox);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skybox);
    // copy data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), &vertexData, GL_STATIC_DRAW);

    // create array object
    glGenVertexArrays(1, &m_vao_skybox);
    // Associate earlier Vertex Buffer with this VAO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skybox);
    //Start recording buffer and attribute metadata
    glBindVertexArray(m_vao_skybox);

    // construct and fill IBO with data
    glGenBuffers(1, &m_ibo_skybox);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo_skybox);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesData), indicesData, GL_STATIC_DRAW);

    // Enable and specify vertex attribute 0 (2 floats )
    // void* cast is necessary for legacy reasons
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat),  (void*)0);


    //create schaderprogramm
    m_prog_skybox= new QOpenGLShaderProgram();

    //AddShader() compiles and attaches shader stages for linking
    m_prog_skybox->addShaderFromSourceFile(QOpenGLShader::Fragment,":/skybox.frag");
    m_prog_skybox->addShaderFromSourceFile(QOpenGLShader::Vertex,":/skybox.vert");
    m_prog_skybox->link();

    //Test linkage
    Q_ASSERT(m_prog_skybox->link());

    glBindVertexArray(0);


}

void Skybox::draw(const QMatrix4x4 &projection, QMatrix4x4 view){

    // temporarily disable writing depth values
    glDepthMask(GL_FALSE);

    // Erase translation in view matrix
    view.column(3) = QVector4D(0.0, 0.0, 0.0, 0.0) ;
    view.scale(10.0);

    // Bind VAO, shader, cube texture
    //Bind texture
     m_prog_skybox->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex);
    glBindVertexArray(m_vao_skybox);



    // Bind Program and all uniforms
    m_prog_skybox->setUniformValue(0, projection);
    m_prog_skybox->setUniformValue(1, view);
    m_prog_skybox->setUniformValue(8, 0);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // Release VAO
     glBindVertexArray(0);

    // Turn depth writes back on
    glDepthMask(GL_TRUE);

}
