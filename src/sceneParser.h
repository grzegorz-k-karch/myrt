#ifndef SCENE_PARSE_H
#define SCENE_PARSE_H

#include <string>

#include "util/statusCode.h"
#include "scene.h"

StatusCode parseScene(std::string sceneFile, Scene& scene);

#endif//SCENE_PARSE_H
