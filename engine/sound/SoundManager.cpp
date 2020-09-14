#include "SoundManager.hpp"

#include <iostream>

SoundManager::SoundManager() {}
SoundManager::~SoundManager() {
    while(Mix_Init(0)){
        Mix_Quit();
    }
}

void SoundManager::init(void (*finishedCallback)(int)) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        printf("Mix_OpenAudio: %s\n", Mix_GetError());

    // Callback for tracking which sounds are on which channels
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
    // Don't play anything if it is a server
    if(!sound || _server)
        return;

    int tmpChannel = -1;
    if (sound->mFadeInMs > 0) {
        if (sound->mLengthMs > 0)
            // Play sound with fade in and timeout
            tmpChannel = Mix_FadeInChannelTimed(
                -1, sound->mMixChunk, sound->mNLoops, sound->mLengthMs, sound->mFadeInMs);
        else
            // Play sound with fade in
            tmpChannel = Mix_FadeInChannel(
                -1, sound->mMixChunk, sound->mNLoops, sound->mFadeInMs);
    } else {
        if (sound->mLengthMs > 0)
            // Play sound with timeout
            tmpChannel = Mix_PlayChannelTimed(
                -1, sound->mMixChunk, sound->mNLoops, sound->mLengthMs);
        else
            // Play sound
            tmpChannel = Mix_PlayChannel(-1, sound->mMixChunk, sound->mNLoops);
    }

    // Check the sound is playing
    if (tmpChannel == -1) {
        std::cout << "Mix_PlayChannel: " << Mix_GetError() << std::endl;
        return;
    }

    // Set sound volume
    int soundVolume = sound->mVolume * mMasterVolume / 128;
    Mix_Volume(tmpChannel, soundVolume);

    // Add positional audio effect
    Mix_SetPosition(tmpChannel, sound->mAngle, sound->mDistance);

    // Store channel of the sound
    sound->channel = tmpChannel;
    channelToSound[tmpChannel] = sound;
}

void SoundManager::channelFinishedCallback(int channel) {
    // Obtain sound from channelToSound map
    std::shared_ptr<Sound> sound = channelToSound[channel].lock();

    // Remove the channelToSound entry
    channelToSound.erase(channel);

    if (!sound)
        return;

    // Call callback if the sound has one
    if (sound->onFinishedPlaying != nullptr)
        sound->onFinishedPlaying();

}

void SoundManager::setMasterVolume(int volume) {
    mMasterVolume = volume;

    // Adjust the volume of all channels
    for(std::map<int, std::weak_ptr<Sound>>::iterator iter = channelToSound.begin(); iter != channelToSound.end(); ++iter) {
        int soundChannel = iter->first;
        int newVolume = iter->second.lock()->mVolume * mMasterVolume / 128;
        Mix_Volume(soundChannel, newVolume);
    }
}
