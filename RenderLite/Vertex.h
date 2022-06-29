#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QVector2D>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    QVector3D Position;
    // normal
    QVector3D Normal;
    // texCoords
    QVector2D TexCoords;
    // tangentÕýÇÐ
    QVector3D Tangent;
    // bitangentË«ÇÐÏß
    QVector3D Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

#endif // VERTEX_H
