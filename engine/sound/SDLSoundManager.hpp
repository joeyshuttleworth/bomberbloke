#ifndef SDLSOUNDMANAGER_HPP
#define SDLSOUNDMANAGER_HPP

#include <SDL_mixer.h>
#include <string>
#include <map>
#include <memory>

#include "ISoundManager.hpp"

#include "Sound.hpp"


class SDLSoundChunk;
class SDLSound;

class SDLSoundManager : public ISoundManager {
private:
     /**
     * Map object containing Mix_Chunk object (sound files). Indexed by sound
     * name (usually the stem of the filename).
     */
  // TODO rename. This is data stored in memory, not file handlers
  std::map<std::string, std::unique_ptr<SDLSoundChunk>> mSoundFileBank;

    /**
     * Map from channel number to currently playing Sound
     */
    std::map<int, SDLSound*> mChannelToSound;

  /**
   * Callback function when channel is finished
   * ONLY call in finishedCallback function (see init)
   */
  void channelFinishedCallback(int channel) override;

  static const int SOUND_FREQUENCY = 44100;
  static const Uint16 SOUND_FORMAT = AUDIO_S16SYS;
  static const int SOUND_N_CHANNELS = 2;
  static const int SOUND_CHUNKSIZE = 1024;

public:
    /**
     * Initialisation: must be called before loading sounds
     */
    static void init(void (*finishedCallback)(int));

  /**
   * Play sound object.
   */
    void playSound(Sound* sound) override;
    /**
     * Loads sound file into soundFileBank.
     */
    void loadFromPath(const std::string& path, const std::string& id) override;

    /**
     * Create Sound object from sound name.
     */
  std::unique_ptr<Sound> createSound(const std::string& soundName) override;

    /**
     * Sets the volume - the volume applied to all channels.
     *
     * @param volume  0-128 where 128 is the maximum volume.
     * @param group   Sound group to change the volume of.
     */
    void setVolume(int volume, SoundGroup group=SOUND_MASTER) override;

    void destroySound(Sound* sound);

    SDLSoundManager();
    ~SDLSoundManager();
};

#endif
