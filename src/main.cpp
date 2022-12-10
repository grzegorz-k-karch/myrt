#include <iostream>
#include "util/programArgs.h"
#include "util/statusCode.h"
#include "util/logging.h"

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
  
  // setup scene on device
  // render
  // save image to disk
  
  return 0;
}
