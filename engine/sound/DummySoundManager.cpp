#include "DummySoundManager.hpp"

#include <iostream>


DummySoundManager::~DummySoundManager()
{
  // Free mix chunks
  // for (auto & iter : soundFileBank) {
  //   Mix_FreeChunk(iter.second);
  // }

  // Quit SDL mixer
  // while (Mix_Init(0)) {
  //   Mix_Quit();
  // }
}

void
DummySoundManager::loadFromPath(const std::string&, const std::string&)
{
  // Mix_Chunk* sound = Mix_LoadWAV_RW(src, 1);
  // if (sound == nullptr)
  //   std::cout << Mix_GetError() << std::endl;

  // // Add file to sound file bank
  // soundFileBank.insert(std::make_pair(id, std::move(sound)));
}

std::unique_ptr<Sound>
DummySoundManager::createSound(const std::string&)
{
  return nullptr;
}

void
DummySoundManager::playSound(Sound*)
{
}

void
DummySoundManager::channelFinishedCallback(int)
{
}

void
DummySoundManager::setVolume(int volume, SoundGroup group)
{
  // Set volume
  if (group == SOUND_MASTER) {
    mMasterVolume = volume;
  } else if (group == SOUND_FX) {
    mFxVolume = volume;
  } else if (group == SOUND_MUSIC) {
    mMusicVolume = volume;
  }
}
