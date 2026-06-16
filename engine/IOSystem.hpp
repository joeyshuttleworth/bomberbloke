#ifndef IOSYSTEM_HPP
#define IOSYSTEM_HPP

#include "DummyGraphicsManager.hpp"
#include "DummySoundManager.hpp"
#include "DummyInputManager.hpp"

class IGraphicsManager;
class ISoundManager;
class IInputManager;

class IOSystem{
public:

  IOSystem(std::unique_ptr<IGraphicsManager> gfx, std::unique_ptr<ISoundManager> smng,
           std::unique_ptr<IInputManager> input)
    :
    mpGraphicsManager(std::move(gfx)),
    mpSoundManager(std::move(smng)),
    mpInputManager(std::move(input))
  {
    mpSoundManager->init();
    mpGraphicsManager->init();
    mpInputManager->init();
  }

  IOSystem() :
    IOSystem(std::make_unique<DummyGraphicsManager>(),
             std::make_unique<DummySoundManager>(),
             std::make_unique<DummyInputManager>())
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

  IInputManager& getInputManager(){
    return *mpInputManager;
  }


private:
  const std::unique_ptr<IGraphicsManager> mpGraphicsManager = nullptr;
  const std::unique_ptr<ISoundManager> mpSoundManager = nullptr;
  const std::unique_ptr<IInputManager> mpInputManager = nullptr;
};

extern IOSystem _fallback_IO_system;

#endif
