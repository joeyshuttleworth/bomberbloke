#include "engine.hpp"
#include "Camera.hpp"

void Camera::rumble(double amplitude, double timeout){
  log_message(DEBUG, "Rumbling!");
  mRumbleTimeout = timeout;
  mRumbleAmplitude = amplitude;
}
