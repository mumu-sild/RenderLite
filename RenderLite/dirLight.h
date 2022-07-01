#ifndef DIRLIGHT_H
#define DIRLIGHT_H
#include <QVector3D>

class DirLight
{
public:
    static float ambient;
    static float diffuse;
    static float specular;
    const QVector3D &getDirection() const;
    void setDirectionX(const float x);
    void setDirectionY(const float y);
    void setDirectionZ(const float z);

    const QVector3D &getColor() const;
    void setColorR(const float r);
    void setColorG(const float g);
    void setColorB(const float b);

private:
    QVector3D direction;
    QVector3D color;
    bool dirLightActivated = true;
};

#endif // DIRLIGHT_H
