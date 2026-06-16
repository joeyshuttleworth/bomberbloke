#ifndef ISOUNDMANAGER_HPP
#define ISOUNDMANAGER_HPP

#include <string>
#include <map>
#include <memory>

#include "Sound.hpp"

class ISoundManager {
protected:
    // Volume applied to all channels.
    int mMasterVolume = 128;

    // Volume applied to FX sound group.
    int mFxVolume = 128;

    // Volume applied to music sound group.
    int mMusicVolume = 128;

    bool mDebug = false;

public:


  ISoundManager(ISoundManager& other) = delete;
  ISoundManager(ISoundManager&& other) = delete;

  ISoundManager& operator=(ISoundManager&) = delete;

    virtual void initCallback(void (*)(int)){}

    virtual void init(){};

    /**
     * Loads sound file into soundFileBank. Returns sound name to use when
     * creating Sound objects (see createSound).
     */
    virtual void loadFromPath(const std::string&, const std::string&){}

    /**
     * Play sound object.
     */
  virtual void playSound(std::shared_ptr<Sound>){};

  virtual void destroySound(Sound*){};

  virtual std::shared_ptr<Sound> createSound(const std::string&) = 0;

  /**
     * Sets the volume - the volume applied to all channels.
     *
     * @param volume  0-128 where 128 is the maximum volume.
     * @param group   Sound group to change the volume of.
     */
    virtual void setVolume(int, SoundGroup =SOUND_MASTER){}

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

  virtual std::shared_ptr<Sound> cloneSound(Sound&) = 0;

   ISoundManager(bool debug=false) : mDebug(debug){};
  ~ISoundManager(){};
};

#endif
