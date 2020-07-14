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
public:
    
    /**
     * Initialisation: must be called before loading sounds
     */
    void init();
    
    /**
     * Loads sound file into soundFileBank. Returns sound name to use when
     * creating Sound objects (see createSound).
     */
    std::string loadFromPath(std::string path);
    
    /**
     * Create Sound object from sound name.
     */
    std::shared_ptr<Sound> createSound(std::string soundName);
    
    /**
     * Play sound object.
     */
    void playSound(Sound &sound);
    
    SoundManager();
};

#endif
