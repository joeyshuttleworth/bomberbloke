#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SDL_mixer.h>
#include <string>
#include <map>
#include <memory>

#include "ISoundManager.hpp"

#include "Sound.hpp"

extern const int SOUND_FREQUENCY;
extern const Uint16 SOUND_FORMAT;
extern const int SOUND_N_CHANNELS;
extern const int SOUND_CHUNKSIZE;


struct SDLMixChunk : SoundChunk{
public:
  SDLMixChunk(Mix_Chunk *chnk = nullptr) : mMixChunk(chnk){
  };

  ~SDLMixChunk(){
    if(mMixChunk)
      Mix_FreeChunk(mMixChunk);
  }

  Mix_Chunk* mMixChunk;
};


class SDLSoundManager {
private:
     /**
     * Map object containing Mix_Chunk object (sound files). Indexed by sound
     * name (usually the stem of the filename).
     */
    std::map<std::string, SDLMixChunk> soundFileBank;

    /**
     * Map from channel number to currently playing Sound
     */
    std::map<int, std::shared_ptr<Sound>> channelToSound;

  /**
   * Callback function when channel is finished
   * ONLY call in finishedCallback function (see init)
   */
  void channelFinishedCallback(int channel) override;

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
     * Loads sound file into soundFileBank.
     */
    void loadFromPath(std::string path, const std::string& id) override;

    /**
     * Create Sound object from sound name.
     */
    std::shared_ptr<Sound> createSound(const std::string& soundName) override;

     /**
     * Play sound object.
     */


    /**
     * Sets the volume - the volume applied to all channels.
     *
     * @param volume  0-128 where 128 is the maximum volume.
     * @param group   Sound group to change the volume of.
     */
    void setVolume(int volume, SoundGroup group=SOUND_MASTER) override;

    SDLSoundManager();
    ~SDLSoundManager();
};

#endif
