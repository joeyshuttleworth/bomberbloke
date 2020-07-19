#ifndef SOUND_HPP
#define SOUND_HPP

#include <SDL2/SDL_mixer.h>
#include <string>
#include <map>

class Sound {
public:
    /**
     * Sound file in Mix_Chunk format.
     */
    Mix_Chunk *mMixChunk;
    
    /**
     * Mixer channel number 
     */
    int channel = -1;
    
    /**
     * Number of loops to play
     */
    int mNLoops = 0;
     */
    void (*onFinishedPlaying)() = nullptr;
    
    /**
     * Initialisation
     */
    Sound() {}
    Sound(Mix_Chunk *soundFile) {
        mMixChunk = soundFile;
    }
    
    
    /**
     */
    
    
};

#endif