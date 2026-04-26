#ifndef SOUND_HPP
#define SOUND_HPP

#include <SDL_mixer.h>
#include <string>
#include <functional>
#include <map>

#include "AbstractSound.hpp"


class AbstractSoundChunk;

class SDLSound {
public:
    /**
     * Initialisation.
     */
    SDLSound() {}
  SDLSound(Mix_Chunk* sound_chunk) : mMixChunk(sound_chunk){
    }

    /**
     * Pauses sound.
     */
    void pause() override {
        if (channel >= 0)
            Mix_Pause(channel);
    }

    /**
     * Resumes sound.
     */
    void resume() override {
        if (channel >= 0)
            Mix_Resume(channel);
    }

    /**
     * Stops sound.
     */
    void stop(int waitMs=0) override {
        if (channel >= 0) {
            if (waitMs > 0) {
                Mix_ExpireChannel(channel, waitMs);
            } else {
                Mix_HaltChannel(channel);
            }
        }
    }

    /**
     * Begins fade out effect at time of call. ms is the number of milliseconds
     * that the fade-out effect should take to go to silence.
     */
    void fadeOut(int ms) override {
        if (channel >= 0)
            Mix_FadeOutChannel(channel, ms);
    }

    /**
     * Returns the length of the sound file.
     */
    int getLengthMs() override {
        return mMixChunk->alen * 1000 / (SOUND_FREQUENCY * SOUND_N_CHANNELS * 2);
    }

protected:
  Mix_Chunk mMixChunk;
};

#endif
