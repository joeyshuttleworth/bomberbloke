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
    /**
     * Maxium length of sound to play
     */
    int mLengthMs = 0;
    /**
     * Milliseconds of time the fade-in effect should take
     */
    int mFadeInMs = 0;
    /**
     * Distance value which controls an effect emulating the attenuation of
     * volume due to distance. Ranges from 0 (close/loud) to 255 (far/quiet)
     */
    int mDistance = 0;
    /**
     * Angle value which controls an effect which pans audio to emulate an audio
     * source in 3D space. The angle value is in degrees (% 360) where 0 is
     * directly in front, 90 is directly right, etc.
     */
    int mAngle = 0;
    /**
     * Callback function for when sound is finished playing
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