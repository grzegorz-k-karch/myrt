#include <iostream>
#include <iostream>
#include "util/logging.h"
#include "sceneParser.h"

#include <boost/property_tree/xml_parser.hpp>
namespace pt = boost::property_tree;


StatusCode checkScene(std::string sceneFile)
{
  return StatusCode::NoError;
}


StatusCode parseScene(std::string sceneFile, Scene &scene)
{
  StatusCode status = StatusCode::NoError;

  /// Check if scene contains camera and at least one renderable object
  status = checkScene(sceneFile);
  if (status != StatusCode::NoError) {
    LOG_TRIVIAL(error) << "Scene invalid.";
  }

  if (status == StatusCode::NoError) {

  }

  return StatusCode::NoError;
}
