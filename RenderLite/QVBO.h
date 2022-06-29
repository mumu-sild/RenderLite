#ifndef QVBO_H
#define QVBO_H

#include "logo.h"
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

typedef unsigned int GLunit;
typedef float GLfloat;
class QVBO : protected QOpenGLFunctions
{
public:
   QVBO(Logo& logo);
   ~QVBO();
   void render();
   Logo logo;
protected:
   QOpenGLVertexArrayObject vao;

};

#endif // QVBO_H
