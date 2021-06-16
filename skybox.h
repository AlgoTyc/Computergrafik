#ifndef SKYBOX_H
#define SKYBOX_H
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>

class Skybox: QOpenGLFunctions_3_3_Core
{
public:
    Skybox();
    ~Skybox();
    void init_skybox();
    void draw(const QMatrix4x4 &projection, QMatrix4x4 view);
    GLuint handleCubeMap() const {
        return m_cubeTex;
    }
private:
    GLuint m_cubeTex;
    QOpenGLShaderProgram* m_prog_skybox;
    GLuint m_vbo_skybox;
    GLuint m_vao_skybox;
    GLuint m_ibo_skybox;


};

#endif // SKYBOX_H
