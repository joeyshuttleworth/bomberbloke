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

    const double width = scene->mDimmension[0];
    const double height = scene->mDimmension[1];

    std::max(width, height);

    int screen_dims[2] = {_window_size[0], _window_size[1]};

    mZoom = std::min(screen_dims[0]/width, screen_dims[1]/height);

    mPosition[0]=width/2;
    mPosition[1]=height/2;

  }
};

#endif
