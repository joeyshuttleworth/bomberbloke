#ifndef ABSTRACTBOT_HPP
#define ABSTRACTBOT_HPP

#include "NetClient.hpp"
#include "scene.hpp"
#include <fstream>

class AbstractBot{
protected:
  std::ofstream mOutputFile();
  std::vector<std::string> mCommands;
  int mState;
  int mActorId = -1;
  std::string mNickname;
  std::string mDescription;

  std::shared_ptr<scene> mpScene;
  std::shared_ptr<actor> mpActor;

  unsigned int mLastHandledTick=0;
  unsigned int mPollDelay = 0;

public:
  virtual void handleCommand(std::vector<std::string>){}

  virtual void update(unsigned int){};

  virtual void handleReset(){};
  virtual void handleServerMessage(std::string){};

  void connectScene(std::shared_ptr<scene>);

  bool isConnectedToScene(){return mpScene!=nullptr;}

  scene* getScene(){
    if(!isConnectedToScene())
      return nullptr;

    return mpScene.get();
  }

  std::string getInfo(){
    return mDescription;
  }

  std::string getNickname(){
    return mNickname;
  }

  std::unique_ptr<NetClient> mpNetClient;

  AbstractBot(){}

  AbstractBot(std::string, std::shared_ptr<scene>, std::shared_ptr<actor>,
              std::unique_ptr<NetClient>);

  AbstractBot(AbstractBot&) = delete;
  AbstractBot operator=(actor&) = delete;
  AbstractBot& operator=(actor&&) = delete;

  int getActorId();

  virtual ~AbstractBot();

};

#endif
