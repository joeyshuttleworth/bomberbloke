#include "SDLSoundManager.hpp"
#include "SDLSound.hpp"
#include "engine.hpp"

#include <iostream>
#include <sstream>

#ifdef __EMSCRIPTEN__
#else
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(files);
#endif

const int SOUND_FREQUENCY = 44100;
const Uint16 SOUND_FORMAT = AUDIO_S16SYS;
const int SOUND_N_CHANNELS = 2;
const int SOUND_CHUNKSIZE = 1024;


SDLSoundManager* SDLSoundManager::instance = nullptr;

SDLSoundManager::SDLSoundManager() = default;
SDLSoundManager::~SDLSoundManager()
{
  // Quit SDL mixer
  while (Mix_Init(0)) {
    Mix_Quit();
  }
}

void
SDLSoundManager::init(){
  if (Mix_OpenAudio(
                    SOUND_FREQUENCY, SOUND_FORMAT, SOUND_N_CHANNELS, SOUND_CHUNKSIZE) == -1){
    std::stringstream strm;

    strm << "Mix_OpenAudio:" << Mix_GetError();
    log_message(ERR, strm.str());
  }

  instance = this;

  Mix_ChannelFinished(channelFinishedCallback);
}

void
SDLSoundManager::loadFromPath(const std::string& path, const std::string& id)
{
  printf("%s, %s\n", path.c_str(), id.c_str());

  Mix_Chunk* sound = nullptr;

  #ifdef __EMSCRIPTEN__
  #else
  auto fs = cmrc::files::get_filesystem();
  auto file = fs.open(path);
  SDL_RWops *io = SDL_RWFromConstMem(file.begin(), file.end() - file.begin());
  sound = Mix_LoadWAV_RW(io, 1);

  if (sound == nullptr){
    std::cout << Mix_GetError() << std::endl;
    return;
  }
  // Add file to sound file bank
  mSoundFileBank[id] = std::make_unique<SDLSoundChunk>(sound);
  #endif
}

std::unique_ptr<Sound>
SDLSoundManager::createSound(const std::string& soundName)
{
   std::unique_ptr<Sound> sound = std::make_unique<SDLSound>(mSoundFileBank[soundName].get());
   sound->onFinishedPlaying = nullptr;

   return sound;
}

void
SDLSoundManager::playSound(Sound* _sound)
{
  if (!_sound)
    return;

  SDLSound* sound = (SDLSound*) _sound;

  if(!sound)
    return;

  if(!sound->mpMixChunk)
    return;

  if(!sound->mpMixChunk->mpMixChunk)
    return;

  int tmpChannel = -1;
  if (sound->mFadeInMs > 0) {
    if (sound->mMaxLengthMs > 0)
      // Play sound with fade in and timeout
      tmpChannel = Mix_FadeInChannelTimed(-1,
                                          sound->mpMixChunk->mpMixChunk,
                                          sound->mNLoops,
                                          sound->mMaxLengthMs,
                                          sound->mFadeInMs);
    else
      // Play sound with fade in
      tmpChannel = Mix_FadeInChannel(
        -1, sound->mpMixChunk->mpMixChunk, sound->mNLoops, sound->mFadeInMs);
  } else {
    if (sound->mMaxLengthMs > 0)
      // Play sound with timeout
      tmpChannel = Mix_PlayChannelTimed(
        -1, sound->mpMixChunk->mpMixChunk, sound->mNLoops, sound->mMaxLengthMs);
    else
      // Play sound
      tmpChannel = Mix_PlayChannel(-1, sound->mpMixChunk->mpMixChunk, sound->mNLoops);
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
  mChannelToSound[tmpChannel] = sound;
}

void
SDLSoundManager::channelFinishedCallback(int channel)
{

  if(!instance)
    return;

  // Obtain sound from mChannelToSound map
  SDLSound* sound = instance->mChannelToSound[channel];

  if(!sound)
    return;

  if (sound->onFinishedPlaying != nullptr) {
    // Call callback if the sound has one
    sound->onFinishedPlaying();
  }

  if (!sound) {
    // Remove the mChannelToSound entry
    instance->mChannelToSound.erase(channel);
    return;
  }

}

void
SDLSoundManager::setVolume(int volume, SoundGroup group)
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
    mChannelToSound.begin();
       iter != mChannelToSound.end();
       ++iter) {
    int soundChannel = iter->first;
    int newVolume = mMasterVolume;

    SDLSound* sound = iter->second;

    if(!sound)
      continue;

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

