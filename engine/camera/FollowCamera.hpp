#ifndef FOLLOWCAMERA_HPP
#define FOLLOWCAMERA_HPP

#include "Camera.hpp"

#include <array>
#include <memory>

class actor;

const double CAMERA_ACCELERATION = 50;
const double CAMERA_MAX_SPEED = 50;
const double CAMERA_MAX_DISTANCE = 10;

class FollowCamera: public Camera {
public:
    // Subject that is followed by the camera
    std::weak_ptr<actor> mSubject;

  using Camera::Camera;

    ~FollowCamera(){
      /* We will get a double free if we destroy the texture after SDL_Quit is called */
      if(mpFrameBuffer && !_halt){
        SDL_DestroyTexture(mpFrameBuffer);
      }
    }

    /**
     * Updates camera position and velocity.
     */
     void update() override;
protected:
    // Velocity of camera in scene units.
    std::array<double, 2> mVelocity = {{ 0, 0 }};
};

#endif
