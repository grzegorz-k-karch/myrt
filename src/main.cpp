#include <iostream>
#include "util/programArgs.h"
#include "util/statusCode.h"
#include "util/logging.h"
#include "scene.h"
#include "sceneParser.h"
#include "data_types.h"

int main(int argc, char** argv)
{
  /// Parse command line arguments
  ProgramArgs programArgs;
  StatusCode status = parseArgs(argc, argv, programArgs);

  if (status == StatusCode::NoError) {
    /// Initialize logger with severity level provided by the user (or use
    /// default "info" value).
    status = initLogger(programArgs.logLevel);    
  }
  if (status == StatusCode::NoError) {
    /// Parse scene file
    Scene scene;
    status = parseScene(programArgs.sceneFile, scene);
  }

  Spectrum spec;
  
  // setup scene on device
  // render
  // save image to disk
  
  return 0;
}
