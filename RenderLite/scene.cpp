#include "scene.h"

void Scene::Add(Object *object)
{
    objects.push_back(object);
}

void Scene::Add(PointLight *pointlight)
{
    pointlights.push_back(pointlight);
}

void Scene::Add(QOpenGLShaderProgram *shaderprogram)
{
    shaderPrograms.push_back(shaderprogram);
}

Scene::Scene()
{

}
