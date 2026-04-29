#include "ISoundManager.hpp"

void ISoundManager::channelFinishedForwarder(int channel){
  this->channelFinishedCallback(channel);
}
