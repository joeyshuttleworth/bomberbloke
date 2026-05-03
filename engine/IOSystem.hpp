#ifndef IOSYSTEM_HPP
#define IOSYSTEM_HPP

#include "DummyGraphicsManager.hpp"
#include "DummySoundManager.hpp"
// #include "DummyInputManager.hpp"

class IGraphicsManager;
class ISoundManager;
// class IInputManager;

class IOSystem{
public:

  IOSystem(std::unique_ptr<IGraphicsManager> gfx, std::unique_ptr<ISoundManager> smng):
    mpGraphicsManager(std::move(gfx)),
    mpSoundManager(std::move(smng))
  {
  }

  IOSystem() :
    mpGraphicsManager(std::make_unique<DummyGraphicsManager>()),
    mpSoundManager(std::make_unique<DummySoundManager>())
  {
  }

  ~IOSystem() = default;

  IOSystem(IOSystem&) = delete;

  IOSystem& operator=(IOSystem&) = delete;
  IOSystem operator=(IOSystem) = delete;

  IOSystem& operator=(IOSystem&&) = delete;

  IGraphicsManager& getGraphicsManager(){
    return *mpGraphicsManager;
  }

  ISoundManager& getSoundManager(){
    return *mpSoundManager;
  }

private:
  const std::unique_ptr<IGraphicsManager> mpGraphicsManager = nullptr;
  const std::unique_ptr<ISoundManager> mpSoundManager = nullptr;
  // IInputManager& mpInputManager;
};

extern IOSystem _fallback_IO_system;

#endif
