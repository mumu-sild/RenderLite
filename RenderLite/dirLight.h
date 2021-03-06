#ifndef DIRLIGHT_H
#define DIRLIGHT_H
#include <QVector3D>
#include <QOpenGLShaderProgram>

class DirLight
{
public:
    static float ambient;
    static float diffuse;
    static float specular;
private:
    QVector3D direction;
    QVector3D color;
public:
    bool dirLightActivated = true;
public:
    QVector3D &getDirection();
    void setDirectionX(const float x);
    void setDirectionY(const float y);
    void setDirectionZ(const float z);

    QVector3D &getColor();
    void setColorR(const float r);
    void setColorG(const float g);
    void setColorB(const float b);

//    void setDirLight(DirLight* dirlight, QOpenGLShaderProgram* shader);
    DirLight(QVector3D dir = QVector3D(-0.2f,-1.0f,-0.3f),
             QVector3D color = QVector3D(1,1,1))
        :direction(dir),color(color)
    {

    }

};

#endif // DIRLIGHT_H
