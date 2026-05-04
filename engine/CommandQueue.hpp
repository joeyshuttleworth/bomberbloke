#ifndef COMMANDQUEUE_HPP
#define COMMANDQUEUE_HPP

#include <queue>
#include <mutex>

using Tokens = std::vector<std::string>;

class CommandQueue{
public:
  Tokens pop(){

    Tokens retval = {};

    std::lock_guard<std::mutex> guard(mMutex);

    if(mQueue.size() > 0){
      retval = mQueue.front();
      mQueue.pop();
    }

    return retval;
  }

  void push(Tokens t){
    std::lock_guard<std::mutex> guard(mMutex);
    mQueue.push(t);
  }

  unsigned int size(){
    std::lock_guard<std::mutex> guard(mMutex);
    return mQueue.size();
  }

private:
  std::mutex mMutex;
  std::queue<Tokens> mQueue = {};
};

#endif
