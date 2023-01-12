#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SDL3/SDL_mixer.h>
#include <string>
#include <map>
#include <memory>

#include "Sound.hpp"

extern bool _server;

extern const int SOUND_FREQUENCY;
extern const Uint16 SOUND_FORMAT;
extern const int SOUND_N_CHANNELS;
extern const int SOUND_CHUNKSIZE;


class SoundManager {
private:
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
    std::map<std::string, Mix_Chunk*> soundFileBank;

    /**
     * Map from channel number to currently playing Sound
     */
    std::map<int, std::shared_ptr<Sound>> channelToSound;

public:
    /**
     * Initialisation: must be called before loading sounds
     */
    static void init(void (*finishedCallback)(int));

    /**
     * Loads sound file into soundFileBank. Returns sound name to use when
     * creating Sound objects (see createSound).
     */
    void loadFromPath(const std::string& path, const std::string& id);

    /**
     * Create Sound object from sound name.
     */
    std::shared_ptr<Sound> createSound(const std::string& soundName);

    /**
     * Returns Mix_Chunk with a given name.
     */
    Mix_Chunk *getSoundFile(std::string soundName);

    /**
     * Play sound object.
     */
    void playSound(const std::shared_ptr<Sound>& sound);

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

    SoundManager();
    ~SoundManager();
};

#endif
