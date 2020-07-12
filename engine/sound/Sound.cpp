#include "Sound.hpp"

Sound::Sound(Mix_Chunk *soundFile) {
    mMixChunk = soundFile;
}

void Sound::loop(int nLoops) {
    mNLoops = nLoops;
}