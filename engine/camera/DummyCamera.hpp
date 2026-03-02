#ifndef ABSTRACTCAMERA_HPP
#define ABSTRACTCAMERA_HPP
#include "AbstractCamera.hpp"

class scene;

class DummyCamera : AbstractCamera{
public:
  DummyCamera::DummyCamera(std::shared_ptr<scene> s){
    mpScene = _scene;
    return;
  }
  virtual ~DummyCamera(){};
};

DummyCamera::~DummyCamera(){}

#endif
