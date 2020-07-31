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
    std::shared_ptr<Sound> newSound = std::make_shared<Sound>(soundFileBank[soundName]);
    return newSound;
}

void SoundManager::playSound(std::shared_ptr<Sound> sound) {
    if(!sound)
        return;

    int tmpChannel = -1;

    if (sound->mFadeInMs > 0) {
        if (sound->mLengthMs > 0)
            tmpChannel = Mix_FadeInChannelTimed(
                -1, sound->mMixChunk, sound->mNLoops, sound->mLengthMs, sound->mFadeInMs);
        else
            tmpChannel = Mix_FadeInChannel(
                -1, sound->mMixChunk, sound->mNLoops, sound->mFadeInMs);
    } else {
        if (sound->mLengthMs > 0)
            tmpChannel = Mix_PlayChannelTimed(
                -1, sound->mMixChunk, sound->mNLoops, sound->mLengthMs);
        else
            tmpChannel = Mix_PlayChannel(-1, sound->mMixChunk, sound->mNLoops);
    }

    if (tmpChannel == -1) {
        std::cout << Mix_GetError() << std::endl;
        return;
    }

    Mix_SetPosition(tmpChannel, sound->mAngle, sound->mDistance);

    sound->channel = tmpChannel;
    channelToSound[tmpChannel] = sound;
}

void SoundManager::channelFinishedCallback(int channel) {
    auto sound = channelToSound[channel];
    channelToSound.erase(channel);

    if (sound->onFinishedPlaying != nullptr)
        sound->onFinishedPlaying();
}
