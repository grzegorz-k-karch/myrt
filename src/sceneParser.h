#ifndef SCENE_PARSE_H
#define SCENE_PARSE_H

#include <string>

#include "StatusCode.h"
#include "Scene.h"

StatusCode checkScene(std::string sceneFile);
StatusCode parseScene(std::string sceneFile, Scene& scene);

#endif//SCENE_PARSE_H
