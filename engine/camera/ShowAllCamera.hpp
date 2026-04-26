#ifndef SHOWALLCAMERA_HPP
#define SHOWALLCAMERA_HPP

#include "Camera.hpp"

#include <array>
#include <memory>

class actor;

class ShowAllCamera: public Camera {
public:

    using Camera::Camera;

  void init(){
    auto scene =  mpScene;

    if(!scene)
      return;

    const double width = scene->mDimension[0];
    const double height = scene->mDimension[1];

    auto dimensions = mrGraphicsManager.getScreenDimensions();

    auto screen_width = dimensions[0];
    auto screen_height = dimensions[1];

    mZoom = std::min(screen_width/width, screen_height/height);

    mPosition[0]=width/2;
    mPosition[1]=height/2;
  }
};

#endif
