#include "engine.hpp"
#include "Camera.hpp"

void Camera::rumble(double amplitude, double timeout){
  mRumbleTimeout = timeout;
  mRumbleAmplitude = amplitude;
  return;
}
