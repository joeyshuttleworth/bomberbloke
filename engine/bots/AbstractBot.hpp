#ifndef ABSTRACTBOT_HPP
#define ABSTRACTBOT_HPP

#include "NetClient.hpp"
#include "scene.hpp"
#include <fstream>

class AbstractBot{
protected:
  std::ofstream mOutputFile();
  std::vector<std::string> mCommands;
  int mState = 0;
  int mActorId = -1;
  std::string mNickname;
  std::string mDescription;

  std::shared_ptr<scene> mpScene = nullptr;
  std::shared_ptr<actor> mpActor = nullptr;

  unsigned int mLastHandledTick=0;
  unsigned int mPollDelay = 0;

public:
  virtual void handleCommand(std::vector<std::string>){}
  virtual void update(unsigned int){};

  virtual void handleReset(){};
  virtual void handleServerMessage(std::string){};

  void connectScene(std::shared_ptr<scene>);

  bool isConnectedToScene(){return mpScene!=nullptr;}

  std::string getInfo(){
    return mDescription;
  }

  std::string getNickname(){
    return mNickname;
  }

  std::unique_ptr<NetClient> mpNetClient = nullptr;

  AbstractBot(){}

  AbstractBot(std::string, std::shared_ptr<scene>, std::shared_ptr<actor>, std::unique_ptr<NetClient>);

  virtual ~AbstractBot();

};

#endif
