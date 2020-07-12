#include "SoundManager.hpp"

#include <iostream>

std::string getStemFromPath(std::string path) {
    std::string stem = "";
    bool onStem = false;
    
    for (std::string::reverse_iterator rit = path.rbegin(); rit != path.rend(); rit++) {
        if (onStem == true) {
            if (*rit == '/' || *rit == '\\')
                break;
            stem.insert(0, 1, *rit);
        } else if (*rit == '.') {
            onStem = true;
        }
    }
    
    return stem;
}

SoundManager::SoundManager() {}

void SoundManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
    }
}

std::string SoundManager::loadFromPath(std::string path) {
    Mix_Chunk *sound = Mix_LoadWAV(path.c_str());
    if (sound == NULL)
        std::cout << Mix_GetError() << std::endl;
    
    // Get stem form filename (e.g. assets/name.wav -> name)
    std::string fileStem = getStemFromPath(path);
    
    // Add file to sound file bank
    soundFileBank.insert(std::make_pair(fileStem, sound));
    
    return fileStem;
}

std::shared_ptr<Sound> SoundManager::createSound(std::string soundName) {
    std::shared_ptr<Sound> newSound(new Sound(soundFileBank[soundName]));
    return newSound;
}

void SoundManager::playSound(Sound &sound) {
    Mix_PlayChannel(-1, sound.mMixChunk, sound.mNLoops);
}