#include <iostream>
#include "logging.h"
#include "scene_parse.h"
#include "shape_utils.h"
#include "material_utils.h"
#include "mesh_utils.h"
#include <iostream>
#include "logging.h"
#include "scene_parse.h"
#include "shape_utils.h"
#include "material_utils.h"
#include "mesh_utils.h"

#include <boost/property_tree/xml_parser.hpp>
namespace pt = boost::property_tree;


StatusCode readScene(
		     std::string sceneFilePath,
		     pt::ptree&  sceneTree
		     )
{
  StatusCode status = StatusCode::NoError;
  // read XML file
  pt::ptree fileTree;
  try {
    pt::read_xml(sceneFilePath, fileTree);
  }
  catch (pt::ptree_error &e) {
    LOG_TRIVIAL(error) << e.what();
    status = StatusCode::FileError;
  }
  if (status == StatusCode::NoError) {
    sceneTree = fileTree.get_child("scene");
  }
  return status;
}


StatusCode checkRequiredShapes(
			       const pt::ptree &sceneTree
			       )
{
  StatusCode status = StatusCode::NoError;

  bool cameraPresent = false;
  bool renderableObjectPresent = false;

  auto node = sceneTree.begin();

  while ((!cameraPresent || !renderableObjectPresent) &&
         node != sceneTree.end()) {
    std::string objectType = node->first;

    if (objectType == "Camera") {
      cameraPresent = true;
    }
    else if (objectType == "Object") {
      renderableObjectPresent = true;
    }
    node++;
  }
  if (!cameraPresent || !renderableObjectPresent) {
    status = StatusCode::SceneError;
  }
  return status;
}


StatusCode checkScene(
		      std::string sceneFilePath
		      )
{
  StatusCode status = StatusCode::NoError;

  pt::ptree sceneTree;
  status = readScene(sceneFilePath, sceneTree);
  if (status == StatusCode::NoError) {
    status = checkRequiredShapes(sceneTree);
  }
  if (status == StatusCode::NoError) {
    LOG_TRIVIAL(info) << "Scene file OK.";
  }
  return status;
}


StatusCode parseCamera(
		       const pt::ptree& sceneTree,
		       Camera&    camera
		       )
{
  auto node = sceneTree.begin();
  while (node != sceneTree.end()) {
    std::string objectType = node->first;
    if (objectType == "Camera") {
      camera = Camera{node->second};
      break;
    }
    node++;
  }
  return StatusCode::NoError;
}


StatusCode getShape(
		    const pt::ptree& objectNode,
		    Shape&           shape
		    )
{
  StatusCode status = StatusCode::NoError;
  auto shapeNode = objectNode.find("Shape");
  if (shapeNode != objectNode.not_found()) {
    std::string shapeKindName = shapeNode->second.get<std::string>("<xmlattr>.type", "");
    ShapeKind shapeKind = stringToShapeKind(shapeKindName);
    if (shapeKind != ShapeKind::None) {
      status = createShape(shapeKind, shapeNode->second, shape);
    }
    else {
      LOG_TRIVIAL(error) << "Unknown shape kind.";
      status = StatusCode::SceneError;
    }
  }
  else {
    LOG_TRIVIAL(error) << "Object does not contain shape.";
    status = StatusCode::SceneError;
  }
  return status;
}


StatusCode getMaterial(
		       const pt::ptree& instanceNode,
		       Material&        material
		       )
{
  StatusCode status = StatusCode::NoError;
  auto materialNode = instanceNode.find("Material");
  if (materialNode != instanceNode.not_found()) {
    std::string materialKindName = materialNode->second.get<std::string>("<xmlattr>.type", "");
    MaterialKind materialKind = stringToMaterialKind(materialKindName);
    if (materialKind != MaterialKind::None) {
      status = createMaterial(materialKind, materialNode->second, material);
    }
  }
  else {
    LOG_TRIVIAL(error) << "Instance does not contain material.";
    status = StatusCode::SceneError;
  }
  return status;
}


StatusCode getTransform(
			const pt::ptree& instanceNode,
			std::array<float, 12>& transform
		       )
{
  StatusCode status = StatusCode::NoError;
  
  float3 translation =
    string2float3(instanceNode.get<std::string>("translation.<xmlattr>.value", "0 0 0"));
  float3 scale = string2float3(instanceNode.get<std::string>("scale.<xmlattr>.value", "1 1 1"));
  float3 rotation = string2float3(instanceNode.get<std::string>("rotation.<xmlattr>.value", "0 0 0"));

  scaleRotateTranslate(scale, rotation, translation, transform);

  return status;
}


StatusCode parseObjects(
			const pt::ptree&               sceneTree,
			std::vector<RenderableObject>& objects
			)
{
  StatusCode status = StatusCode::NoError;
  for (auto sceneIter = sceneTree.begin(); sceneIter != sceneTree.end(); sceneIter++) {
    std::string objectType = sceneIter->first;
    if (objectType != "Object") {
      continue;
    }
    auto objectNode = sceneIter->second;
    Shape shape;
    status = getShape(objectNode, shape);
    if (status == StatusCode::NoError) {
      std::array<float, 12> transform;
      transform.fill(0.0f);
      std::vector<InstanceProps> instanceProps;
      for (auto objectIter = objectNode.begin(); objectIter != objectNode.end(); objectIter++) {
	if (objectIter->first == "Instance") {
	  auto instanceNode = objectIter->second;
	  Material material;
          if (status == StatusCode::NoError) {
            status = getMaterial(instanceNode, material);
          }
          if (status == StatusCode::NoError) {
            status = getTransform(instanceNode, transform);
          }
	  if (status == StatusCode::NoError) {
	    instanceProps.push_back(InstanceProps{transform, material});
          }
        }
      }
      objects.push_back(RenderableObject{shape, instanceProps});
    }
  }
  return status;
}


StatusCode parseScene(
		      std::string                    sceneFilePath,
		      Camera&                        camera,
		      std::vector<RenderableObject>& objects
		      )
{
  StatusCode status = StatusCode::NoError;

  pt::ptree sceneTree;
  status = readScene(sceneFilePath, sceneTree);

  if (status == StatusCode::NoError) {
    status = parseCamera(sceneTree, camera);
  }
  if (status == StatusCode::NoError) {
    status = parseObjects(sceneTree, objects);
  }
  return status;
}

namespace pt = boost::property_tree;


StatusCode readScene(
		     std::string sceneFilePath,
		     pt::ptree&  sceneTree
		     )
{
  StatusCode status = StatusCode::NoError;
  // read XML file
  pt::ptree fileTree;
  try {
    pt::read_xml(sceneFilePath, fileTree);
  }
  catch (pt::ptree_error &e) {
    LOG_TRIVIAL(error) << e.what();
    status = StatusCode::FileError;
  }
  if (status == StatusCode::NoError) {
    sceneTree = fileTree.get_child("scene");
  }
  return status;
}


StatusCode checkRequiredShapes(
			       const pt::ptree &sceneTree
			       )
{
  StatusCode status = StatusCode::NoError;

  bool cameraPresent = false;
  bool renderableObjectPresent = false;

  auto node = sceneTree.begin();

  while ((!cameraPresent || !renderableObjectPresent) &&
         node != sceneTree.end()) {
    std::string objectType = node->first;

    if (objectType == "Camera") {
      cameraPresent = true;
    }
    else if (objectType == "Object") {
      renderableObjectPresent = true;
    }
    node++;
  }
  if (!cameraPresent || !renderableObjectPresent) {
    status = StatusCode::SceneError;
  }
  return status;
}


StatusCode checkScene(
		      std::string sceneFilePath
		      )
{
  StatusCode status = StatusCode::NoError;

  pt::ptree sceneTree;
  status = readScene(sceneFilePath, sceneTree);
  if (status == StatusCode::NoError) {
    status = checkRequiredShapes(sceneTree);
  }
  if (status == StatusCode::NoError) {
    LOG_TRIVIAL(info) << "Scene file OK.";
  }
  return status;
}


StatusCode parseCamera(
		       const pt::ptree& sceneTree,
		       Camera&    camera
		       )
{
  auto node = sceneTree.begin();
  while (node != sceneTree.end()) {
    std::string objectType = node->first;
    if (objectType == "Camera") {
      camera = Camera{node->second};
      break;
    }
    node++;
  }
  return StatusCode::NoError;
}


StatusCode getShape(
		    const pt::ptree& objectNode,
		    Shape&           shape
		    )
{
  StatusCode status = StatusCode::NoError;
  auto shapeNode = objectNode.find("Shape");
  if (shapeNode != objectNode.not_found()) {
    std::string shapeKindName = shapeNode->second.get<std::string>("<xmlattr>.type", "");
    ShapeKind shapeKind = stringToShapeKind(shapeKindName);
    if (shapeKind != ShapeKind::None) {
      status = createShape(shapeKind, shapeNode->second, shape);
    }
    else {
      LOG_TRIVIAL(error) << "Unknown shape kind.";
      status = StatusCode::SceneError;
    }
  }
  else {
    LOG_TRIVIAL(error) << "Object does not contain shape.";
    status = StatusCode::SceneError;
  }
  return status;
}


StatusCode getMaterial(
		       const pt::ptree& instanceNode,
		       Material&        material
		       )
{
  StatusCode status = StatusCode::NoError;
  auto materialNode = instanceNode.find("Material");
  if (materialNode != instanceNode.not_found()) {
    std::string materialKindName = materialNode->second.get<std::string>("<xmlattr>.type", "");
    MaterialKind materialKind = stringToMaterialKind(materialKindName);
    if (materialKind != MaterialKind::None) {
      status = createMaterial(materialKind, materialNode->second, material);
    }
  }
  else {
    LOG_TRIVIAL(error) << "Instance does not contain material.";
    status = StatusCode::SceneError;
  }
  return status;
}


StatusCode getTransform(
			const pt::ptree& instanceNode,
			std::array<float, 12>& transform
		       )
{
  StatusCode status = StatusCode::NoError;
  
  float3 translation =
    string2float3(instanceNode.get<std::string>("translation.<xmlattr>.value", "0 0 0"));
  float3 scale = string2float3(instanceNode.get<std::string>("scale.<xmlattr>.value", "1 1 1"));
  float3 rotation = string2float3(instanceNode.get<std::string>("rotation.<xmlattr>.value", "0 0 0"));

  scaleRotateTranslate(scale, rotation, translation, transform);

  return status;
}


StatusCode parseObjects(
			const pt::ptree&               sceneTree,
			std::vector<RenderableObject>& objects
			)
{
  StatusCode status = StatusCode::NoError;
  for (auto sceneIter = sceneTree.begin(); sceneIter != sceneTree.end(); sceneIter++) {
    std::string objectType = sceneIter->first;
    if (objectType != "Object") {
      continue;
    }
    auto objectNode = sceneIter->second;
    Shape shape;
    status = getShape(objectNode, shape);
    if (status == StatusCode::NoError) {
      std::array<float, 12> transform;
      transform.fill(0.0f);
      std::vector<InstanceProps> instanceProps;
      for (auto objectIter = objectNode.begin(); objectIter != objectNode.end(); objectIter++) {
	if (objectIter->first == "Instance") {
	  auto instanceNode = objectIter->second;
	  Material material;
          if (status == StatusCode::NoError) {
            status = getMaterial(instanceNode, material);
          }
          if (status == StatusCode::NoError) {
            status = getTransform(instanceNode, transform);
          }
	  if (status == StatusCode::NoError) {
	    instanceProps.push_back(InstanceProps{transform, material});
          }
        }
      }
      objects.push_back(RenderableObject{shape, instanceProps});
    }
  }
  return status;
}


StatusCode parseScene(
		      std::string                    sceneFilePath,
		      Camera&                        camera,
		      std::vector<RenderableObject>& objects
		      )
{
  StatusCode status = StatusCode::NoError;

  pt::ptree sceneTree;
  status = readScene(sceneFilePath, sceneTree);

  if (status == StatusCode::NoError) {
    status = parseCamera(sceneTree, camera);
  }
  if (status == StatusCode::NoError) {
    status = parseObjects(sceneTree, objects);
  }
  return status;
}
