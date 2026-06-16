#ifndef DUMMYSOUNDMANAGER_HPP
#define DUMMYSOUNDMANAGER_HPP

#include <SDL_mixer.h>
#include <string>
#include <map>
#include <memory>

#include "ISoundManager.hpp"


class DummySoundManager : public ISoundManager {
protected:
    // Volume applied to all channels.
    int mMasterVolume = 128;

    // Volume applied to FX sound group.
    int mFxVolume = 128;

    // Volume applied to music sound group.
    int mMusicVolume = 128;

    /**
     * Map object containing Mix_Chunk object (sound files). Indexed by sound
     * name (usually the stem of the filename).
     */
  std::map<std::string, std::unique_ptr<SoundChunk>> soundFileBank;

public:
    /**
     * Loads sound file into soundFileBank. Returns sound name to use when
     * creating Sound objects (see createSound).
     */
    void loadFromPath(const std::string& path, const std::string& id);

    /**
     * Play sound object.
     */
  void playSound(std::shared_ptr<Sound>){}

    /**
     * Callback function when channel is finished
     * ONLY call in finishedCallback function (see init)
     */
    void channelFinishedCallback(int channel);

    /**
     * Sets the volume - the volume applied to all channels.
     *
     * @param volume  0-128 where 128 is the maximum volume.
     * @param group   Sound group to change the volume of.
     */
     void setVolume(int volume, SoundGroup group=SOUND_MASTER);

    /**
     * Gets the master volume - the volume applied to all channels.
     */
    int getVolume(SoundGroup group=SOUND_MASTER) {
        if (group == SOUND_FX)
          return mFxVolume;
        else if (group == SOUND_MUSIC)
          return mMusicVolume;
        else
          return mMasterVolume;
    }


  std::shared_ptr<Sound> createSound(const std::string&) override;
  std::shared_ptr<Sound> cloneSound(Sound& sound) override;

    DummySoundManager(){};
    ~DummySoundManager();
};

#endif
