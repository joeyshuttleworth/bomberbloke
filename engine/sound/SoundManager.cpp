#include "SoundManager.hpp"

#include <iostream>

SoundManager::SoundManager() {}

void SoundManager::init(void (*finishedCallback)(int)) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
    
    Mix_ChannelFinished(finishedCallback);
}

void SoundManager::loadFromPath(std::string path, std::string id) {
    Mix_Chunk *sound = Mix_LoadWAV(path.c_str());
    if (sound == NULL)
        std::cout << Mix_GetError() << std::endl;
    
    // Add file to sound file bank
    soundFileBank.insert(std::make_pair(id, sound));
}

std::shared_ptr<Sound> SoundManager::createSound(std::string soundName) {
    std::shared_ptr<Sound> newSound(new Sound(soundFileBank[soundName]));
    return newSound;
}

void SoundManager::playSound(Sound &sound) {
    Mix_PlayChannel(-1, sound.mMixChunk, sound.mNLoops);

void SoundManager::channelFinishedCallback(int channel) {
    Sound *sound = channelToSound[channel];
    channelToSound.erase(channel);
    
    if (sound->onFinishedPlaying != nullptr)
        sound->onFinishedPlaying();
}