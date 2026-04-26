#ifndef DUMMYSOUNDMANAGER_HPP
#define DUMMYSOUNDMANAGER_HPP

#include <string>
#include <map>
#include <memory>
#include <sstream>

#include "engine.hpp"
#include "Sound.hpp"
#include "ISoundManager.hpp"

extern const int SOUND_FREQUENCY;
extern const Uint16 SOUND_FORMAT;
extern const int SOUND_N_CHANNELS;
extern const int SOUND_CHUNKSIZE;


class DummySoundManager : public ISoundManager {
protected:


public:

  void loadFromPath(const std::string& path, const std::string& name) override{
    if(mDebug){
      std::stringstream strm;
      strm << "Loading sound " << name << " from " << path;
      log_message(DEBUG, strm.str());
    }
  }

  void playSound(const Sound& sound) override{
    std::stringstream strm;
    if(mDebug){
      strm << "Loading sound " << sound.mName;
      log_message(DEBUG, strm.str());
      }
  }

  std::shared_ptr<Sound> createSound(const std::string&){
    return nullptr;
  }

    DummySoundManager();
    ~DummySoundManager();
};


DummySoundManager::DummySoundManager(){
}

#endif
