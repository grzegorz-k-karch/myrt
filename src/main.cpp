#include <iostream>
#include "util/programArgs.h"
#include "util/statusCode.h"
#include "util/logging.h"

int main(int argc, char** argv)
{
  StatusCode status = StatusCode::NoError;

  /// Parse command line arguments
  ProgramArgs programArgs;
  status = parseArgs(argc, argv, programArgs);
  if (status != StatusCode::NoError) {
    std::cerr << "Could not parse input arguments. Exiting." << std::endl;
    return -1;
  }

  /// Initialize logger with severity level provided by the user (or use
  /// default "info" value).
  status = initLogger(programArgs.logLevel);
  if (status != StatusCode::NoError) {
    LOG_TRIVIAL(error) << "Logging error. Exiting.";
    return -1;
  }

  /// Check if scene contains camera and at least one renderable object
  status = checkScene(programArgs.sceneFilePath);
  if (status != StatusCode::NoError) {
    LOG_TRIVIAL(error) << "Scene invalid. Exiting.";
    return -1;
  }

  /// Parse scene file
  // setup scene on device
  // render
  // save image to disk
  
  return 0;
}
