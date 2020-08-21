/* Used to fill in the gaps in an actors position
   between server updates */


#ifndef INTERPOLATOR_HPP
#define INTERPOLATOR_HPP
#include<assert.h>
#include<vector>
#include<list>
#include<array>
#include "engine.hpp"

using dvector = std::array<double, 2>;

typedef struct {
  dvector  statePosition;
  dvector velocity;
  int serverTick;
  unsigned int clientTick;
} VectorState;

class Interpolator{
private:
  /* A struct only used in this class to keep track of
     the various positions of the actor at different times
  */
 /* mBuffer is used to store previous positions of the actor
     mBufferSize is the maximum number of states that we remember
     this must be larger than mDelay.
   */
  unsigned int mBufferSize;
  std::list<VectorState> mBuffer;

  /* How many ticks behind the server are we? */
  int mDelay;

public:
  Interpolator(unsigned int buffer_size=10, unsigned int delay=5){
    mBufferSize = buffer_size;
    mDelay = delay;
    assert((int)mBufferSize>=mDelay);
  }

  /* Use this before calling getPos to see if the actor has actually moved
     otherwise getPos does not make sense. Make sure that these functions are
     only ever called in the same thread.
  */
  bool hasMoved(){
    return mBuffer.size()>0;
  }

  void update(){
    remove_if(mBuffer.begin(), mBuffer.end(), [&](VectorState vs) -> bool{
                                                return abs(mBuffer.back().serverTick - vs.serverTick) || _tick - vs.clientTick > mBufferSize;
                                              });
  }

  dvector getPos(){
    /* Use linear interpolation to predict the position
       of the actor mDelay ticks before _ticks
    */
    if(mBuffer.size()==0)
      return {{ .0, .0 }};
    else if(mBuffer.size()==1)
      return mBuffer.front().statePosition;

    std::vector<VectorState> states(mBufferSize);
    std::vector<bool> state_exists(mBufferSize, false); //Set all to false

    int most_recent_tick = mBuffer.back().serverTick;

    VectorState *state1 = nullptr, *state2 = nullptr;

    for(auto i = mBuffer.begin(); i!=mBuffer.end(); i++){
      int ticks_behind = most_recent_tick - i->serverTick + _tick - mBuffer.back().clientTick;
      if(ticks_behind > mDelay)
        state1 = &(*i);
      else if(ticks_behind == mDelay)
        return i->statePosition;
      else{
        state2 = &(*i);
        break;
      }
    }

    if(state1 && !state2){
      dvector rc = mBuffer.back().statePosition;
      // mBuffer.clear();
      return rc;
    }

    else if(!state1 && state2){
      return state2->statePosition;
    }

    int x1 = state1->serverTick;
    int x2 = state2->serverTick;

    dvector y1 = state1->statePosition;
    dvector y2 = state2->statePosition;

    /* Linear interpolation between the two values */
    return {{ y1[0] + (mDelay - x1)*(y2[0]-y1[0])/(x2-x1), y1[1] + (mDelay - x1) * (y2[1]-y1[1])/(x2-x1) }};
  }

  void addState(dvector new_pos, dvector velocity, int tick){
    const VectorState vs = {new_pos, velocity, tick, _tick};
    if(mBuffer.size()==0)
      mBuffer.push_back(vs);
    else{
      bool added = false;
      for(auto i = mBuffer.begin(); i != mBuffer.end(); i++){
        if(i->serverTick > tick){
          mBuffer.insert(i, vs);
          added=true;
          break;
        }
      }
      if(!added)
        mBuffer.push_back(vs);
    }
    if(mBuffer.size() > (unsigned int)mBufferSize){
      mBuffer.pop_front();
    }
    return;
  }

  dvector getVelocity(){
      return {{ 0, 0 }};
    /*  TODO */
  }
};


#endif
