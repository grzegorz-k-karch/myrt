#include "spectrum.h"

int main()
{
  std::vector<Float> lambda = {Float(402), Float(407)};
  std::vector<Float> value = {Float(1), Float(1)};

  Spectrum spec(lambda, value);
  
  return 0;
}
