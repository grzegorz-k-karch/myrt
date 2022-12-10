__global__ void renderScene(Scene* scene, Integrator* integrator)
{
  integrator->render(scene);
}

