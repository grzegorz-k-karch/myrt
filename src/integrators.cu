#include "scene.h"
#include "intergrators.h"

__global__ void renderScene(Scene* scene, Integrator* integrator)
{
  integrator->render(scene);
}

