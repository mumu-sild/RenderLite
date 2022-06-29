#include "scene.h"

void Scene::Add(Object *object)
{
    objects.push_back(object);
}

Scene::Scene()
{

}
