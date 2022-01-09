#include "SoundManager.hpp"

#include <iostream>

const int SOUND_FREQUENCY = 44100;
const Uint16 SOUND_FORMAT = AUDIO_S16SYS;
const int SOUND_N_CHANNELS = 2;
const int SOUND_CHUNKSIZE = 1024;

SoundManager::SoundManager() = default;
SoundManager::~SoundManager()
{
  // Free mix chunks
  for (auto & iter : soundFileBank) {
    Mix_FreeChunk(iter.second);
  }

  // Quit SDL mixer
  while (Mix_Init(0)) {
    Mix_Quit();
  }
}

void
SoundManager::init(void (*finishedCallback)(int))
{
  if (Mix_OpenAudio(
        SOUND_FREQUENCY, SOUND_FORMAT, SOUND_N_CHANNELS, SOUND_CHUNKSIZE) == -1)
    printf("Mix_OpenAudio: %s\n", Mix_GetError());

  // Callback for tracking which sounds are on which channels
  Mix_ChannelFinished(finishedCallback);
}

void
SoundManager::loadFromPath(const std::string& path, const std::string& id)
{
  Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
  if (sound == nullptr)
    std::cout << Mix_GetError() << std::endl;

  // Add file to sound file bank
  soundFileBank.insert(std::make_pair(id, sound));
}

std::shared_ptr<Sound>
SoundManager::createSound(const std::string& soundName)
{
  std::shared_ptr<Sound> newSound =
    std::make_shared<Sound>(soundFileBank[soundName]);
  return newSound;
}

void
SoundManager::playSound(const std::shared_ptr<Sound>& sound)
{
  // Don't play anything if it is a server
  if (!sound || _server)
    return;

  int tmpChannel = -1;
  if (sound->mFadeInMs > 0) {
    if (sound->mMaxLengthMs > 0)
      // Play sound with fade in and timeout
      tmpChannel = Mix_FadeInChannelTimed(-1,
                                          sound->mMixChunk,
                                          sound->mNLoops,
                                          sound->mMaxLengthMs,
                                          sound->mFadeInMs);
    else
      // Play sound with fade in
      tmpChannel = Mix_FadeInChannel(
        -1, sound->mMixChunk, sound->mNLoops, sound->mFadeInMs);
  } else {
    if (sound->mMaxLengthMs > 0)
      // Play sound with timeout
      tmpChannel = Mix_PlayChannelTimed(
        -1, sound->mMixChunk, sound->mNLoops, sound->mMaxLengthMs);
    else
      // Play sound
      tmpChannel = Mix_PlayChannel(-1, sound->mMixChunk, sound->mNLoops);
  }

  // Check the sound is playing
  if (tmpChannel == -1) {
    std::cout << "Mix_PlayChannel: " << Mix_GetError() << std::endl;
    return;
  }

  // Set sound volume
  int soundVolume = sound->mVolume * mMasterVolume / 128;
  if (sound->mGroup == SOUND_FX) {
    soundVolume = soundVolume * mFxVolume / 128;
  } else if (sound->mGroup == SOUND_MUSIC) {
    soundVolume = soundVolume * mMusicVolume / 128;
  }
  Mix_Volume(tmpChannel, soundVolume);

  // Add positional audio effect
  Mix_SetPosition(tmpChannel, sound->mAngle, sound->mDistance);

  // Store channel of the sound
  sound->channel = tmpChannel;
  channelToSound[tmpChannel] = sound;
}

void
SoundManager::channelFinishedCallback(int channel)
{
  // Obtain sound from channelToSound map
  std::shared_ptr<Sound> sound = channelToSound[channel];

  if (!sound) {
    // Remove the channelToSound entry
    channelToSound.erase(channel);
    return;
  }

  if (sound->onFinishedPlaying != nullptr) {
    // Call callback if the sound has one
    sound->onFinishedPlaying();
  }
}

void
SoundManager::setVolume(int volume, SoundGroup group)
{
  // Set volume
  if (group == SOUND_MASTER) {
    mMasterVolume = volume;
  } else if (group == SOUND_FX) {
    mFxVolume = volume;
  } else if (group == SOUND_MUSIC) {
    mMusicVolume = volume;
  }

  // Adjust the volume of all channels
  for (auto iter =
    channelToSound.begin();
       iter != channelToSound.end();
       ++iter) {
    int soundChannel = iter->first;
    int newVolume = mMasterVolume;

    std::shared_ptr<Sound> sound = iter->second;
    if (sound && (sound->mGroup == group || group == SOUND_MASTER)) {
      newVolume = sound->mVolume * mMasterVolume / 128;
      if (group == SOUND_FX) {
        newVolume = newVolume * mFxVolume / 128;
      } else if (group == SOUND_MUSIC) {
        newVolume = newVolume * mMusicVolume / 128;
      }
    }

    Mix_Volume(soundChannel, newVolume);
  }
}
