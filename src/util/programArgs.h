#ifndef ARGS_PARSE_H
#define ARGS_PARSE_H

#include <string>
#include <boost/log/trivial.hpp>

#include "statusCode.h"

struct ProgramArgs {
  std::string sceneFile;
  std::string pictureFile;
  int sampleCount;
  int imageWidth;
  int imageHeight;
  int rayDepth;
  boost::log::trivial::severity_level logLevel;
};

StatusCode parseArgs(int argc, char** argv, ProgramArgs& programArgs);

#endif // ARGS_PARSE_H
