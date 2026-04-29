#include "DummySoundManager.hpp"

#include <iostream>

const int SOUND_FREQUENCY = 44100;
const Uint16 SOUND_FORMAT = AUDIO_S16SYS;
const int SOUND_N_CHANNELS = 2;
const int SOUND_CHUNKSIZE = 1024;

DummySoundManager::DummySoundManager() = default;
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
DummySoundManager::init(void (*finishedCallback)(int))
{
  if (Mix_OpenAudio(
        SOUND_FREQUENCY, SOUND_FORMAT, SOUND_N_CHANNELS, SOUND_CHUNKSIZE) == -1)
    printf("Mix_OpenAudio: %s\n", Mix_GetError());

  // Callback for tracking which sounds are on which channels
  Mix_ChannelFinished(finishedCallback);
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
DummySoundManager::playSound(Sound* sound)
{
  int tmpChannel = -1;

  // if (sound->mFadeInMs > 0) {
  //   if (sound->mMaxLengthMs > 0)
  //     // Play sound with fade in and timeout
  //     // tmpChannel = Mix_FadeInChannelTimed(-1,
  //     //                                     sound->mMixChunk,
  //     //                                     sound->mNLoops,
  //     //                                     sound->mMaxLengthMs,
  //     //                                     sound->mFadeInMs);
  //   else
  //     // Play sound with fade in
  //     // tmpChannel = Mix_FadeInChannel(
  //     //   -1, sound->mMixChunk, sound->mNLoops, sound->mFadeInMs);
  // } else {
  //   if (sound->mMaxLengthMs > 0)
  //     // Play sound with timeout
  //     // tmpChannel = Mix_PlayChannelTimed(
  //     //   -1, sound->mMixChunk, sound->mNLoops, sound->mMaxLengthMs);
  //   else
  //     // Play sound
  //     // tmpChannel = Mix_PlayChannel(-1, sound->mMixChunk, sound->mNLoops);
  // }

  // Check the sound is playing
  if (tmpChannel == -1) {
    std::cout << "Mix_PlayChannel: " << Mix_GetError() << std::endl;
    return;
  }

  // Set sound volume
  int soundVolume = sound->getVolume() * mMasterVolume / 128;
  if (sound->getGroup() == SOUND_FX) {
    soundVolume = soundVolume * mFxVolume / 128;
  } else if (sound->getGroup() == SOUND_MUSIC) {
    soundVolume = soundVolume * mMusicVolume / 128;
  }
  // Mix_Volume(tmpChannel, soundVolume);

  // Add positional audio effect
  // Mix_SetPosition(tmpChannel, sound->mAngle, sound->mDistance);

  // Store channel of the sound
  sound->channel = tmpChannel;
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
