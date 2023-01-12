#ifndef SOUND_HPP
#define SOUND_HPP

#include <SDL3/SDL_mixer.h>
#include <string>
#include <functional>
#include <map>

enum SoundGroup {
    SOUND_MASTER,
    SOUND_MUSIC,
    SOUND_FX
};

extern const int SOUND_FREQUENCY;
extern const int SOUND_N_CHANNELS;

class Sound {
public:
    /**
     * Sound file in Mix_Chunk format.
     */
    Mix_Chunk *mMixChunk;

    /**
     * Mixer channel number.
     */
    int channel = -1;

    /**
     * Group the sound belongs to.
     */
    SoundGroup mGroup = SOUND_MASTER;

    /**
     * Number of loops to play.
     */
    int mNLoops = 0;
    /**
     * Maxium length of sound to play.
     */
    int mMaxLengthMs = 0;
    /**
     * Milliseconds of time the fade-in effect should take.
     */
    int mFadeInMs = 0;
    /**
     * Volume of sound played - ranges from 0 to 128.
     */
    int mVolume = 128;
    /**
     * Distance value which controls an effect emulating the attenuation of
     * volume due to distance. Ranges from 0 (close/loud) to 255 (far/quiet).
     */
    int mDistance = 0;
    /**
     * Angle value which controls an effect which pans audio to emulate an audio
     * source in 3D space. The angle value is in degrees (% 360) where 0 is
     * directly in front, 90 is directly right, etc.
     */
    int mAngle = 0;
    /**
     * Callback function for when sound is finished playing.
     */
    std::function<void()> onFinishedPlaying = nullptr;

    /**
     * Initialisation.
     */
    Sound() {}
    Sound(Mix_Chunk *soundFile) {
        mMixChunk = soundFile;
    }

    /**
     * Pauses sound.
     */
    void pause() {
        if (channel >= 0)
            Mix_Pause(channel);
    }

    /**
     * Resumes sound.
     */
    void resume() {
        if (channel >= 0)
            Mix_Resume(channel);
    }

    /**
     * Stops sound.
     */
    void stop(int waitMs=0) {
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
    void fadeOut(int ms) {
        if (channel >= 0)
            Mix_FadeOutChannel(channel, ms);
    }

    /**
     * Returns the length of the sound file.
     */
    int getLengthMs() {
        return mMixChunk->alen * 1000 / (SOUND_FREQUENCY * SOUND_N_CHANNELS * 2);
    }
};

#endif
