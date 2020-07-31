#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SDL2/SDL_mixer.h>
#include <string>
#include <map>
#include <memory>

#include "Sound.hpp"

class SoundManager {
private:
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
    void init(void (*finishedCallback)(int));

    /**
     * Loads sound file into soundFileBank. Returns sound name to use when
     * creating Sound objects (see createSound).
     */
    void loadFromPath(std::string path, std::string id);

    /**
     * Create Sound object from sound name.
     */
    std::shared_ptr<Sound> createSound(std::string soundName);

    /**
     * Play sound object.
     */
    void playSound(std::shared_ptr<Sound> sound);

    /**
     * Callback function when channel is finished
     * ONLY call in finishedCallback function (see init)
     */
    void channelFinishedCallback(int channel);

    SoundManager();
};

#endif
