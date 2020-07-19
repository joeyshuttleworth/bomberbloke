#ifndef SOUND_HPP
#define SOUND_HPP

#include <SDL2/SDL_mixer.h>
#include <string>
#include <map>

class Sound {
public:
    /**
     * Sets number of loops to play.
     */
    void loop(int nLoops);
    
    /**
     * Initialisation
     */
    Sound() {}
    Sound(Mix_Chunk *soundFile) {
        mMixChunk = soundFile;
    }
    
    
    /**
     * Sound file in Mix_Chunk format.
     */
    Mix_Chunk *mMixChunk;
    
    // Number of loops to play
    int mNLoops = 0;
    
    Sound(Mix_Chunk *soundFile);
};

#endif