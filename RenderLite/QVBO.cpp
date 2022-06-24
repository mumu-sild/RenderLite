#include "QVBO.h"

QVBO::QVBO(Logo& logo):logo(logo){
    initializeOpenGLFunctions();
    GLunit VBO;
    glGenBuffers(1,&VBO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,logo.count()*sizeof(GLfloat),logo.constData(),GL_STATIC_DRAW);

    vao.create();
    vao.bind();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                                 nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                                 reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    vao.release();
}

void QVBO::render(){
    vao.bind();
    glDrawArrays(GL_TRIANGLES,0,logo.vertexCount());
    vao.release();
}
QVBO::~QVBO(){

}

