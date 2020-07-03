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


class Interpolator{
private:
  /* A struct only used in this class to keep track of
     the various positions of the actor at different times
  */
  typedef struct {
    dvector  statePosition;
    dvector velocity;
    int tick;
  } VectorState;

 /* mBuffer is used to store previous positions of the actor
     mBufferSize is the maximum number of states that we remember
     this must be larger than mDelay.
   */
  int mBufferSize;
  std::list<VectorState> mBuffer;

  /* How many ticks behind the server are we? */
  int mDelay;

public:
  Interpolator(unsigned int buffer_size=20, unsigned int delay=10){
    mBufferSize = buffer_size;
    mDelay = delay;
    assert(mBufferSize>=mDelay);
  }

  /* Use this before calling getPos to see if the actor has actually moved
     otherwise getPos does not make sense. Make sure that these functions are
     only ever called in the same thread.
  */
  bool hasMoved(){
    return mBuffer.size()>0;
  }

  dvector getPos(){
    /* Use linear interpolation to predict the position
       of the actor mDelay ticks before _ticks
    */
    if(!hasMoved())
      return {.0,.0};
    else if(mBuffer.size()==1)
      return mBuffer.front().statePosition;

    std::vector<dvector> states(mBufferSize);
    std::vector<bool> state_exists(mBufferSize, false); //Set all to false

    int most_recent_tick = mBuffer.back().tick;

    for(auto i = mBuffer.begin(); i!=mBuffer.end(); i++){
      int ticks_behind = most_recent_tick - i->tick;
      if(ticks_behind == mDelay)
        return i->statePosition;
      assert(ticks_behind < mBufferSize && ticks_behind >= 0);
      states[ticks_behind] = i->statePosition;
      state_exists[ticks_behind] = true;
    }

    bool set1=false;
    int x1;
    dvector y1;
    for(int i = mDelay-1; i > 0; i--){
      if(state_exists[i]){
        set1 = true;
        x1 = i;
        y1 = states[i];
        break;
      }
    }

    bool set2;
    int x2;
    dvector y2;

    for(int i = 0; i < mBufferSize - mDelay; i++){
      if(state_exists[i]){
        set2 = true;
        x2   = i;
        y2   = states[i];
        break;
      }
    }

    if(!set1 || !set2){
      log_message(DEBUG, "No future state to interpolate, returning last known position");
      return mBuffer.back().statePosition;
    }

    return {y1[0] + (mDelay - x1)*(y2[0]-y1[0])/(x2-x1), y1[1] + (mDelay - x1) * (y2[1]-y1[1])/(x2-x1)};
  }

  void addState(dvector new_pos, dvector velocity, int tick){
    for(auto i = mBuffer.begin(); i != mBuffer.end(); i++){
      if(i->tick > tick){
        const VectorState vs = {new_pos, velocity, tick};
        mBuffer.insert(i, vs);
        break;
      }
    }

    if(mBuffer.size() > (unsigned int)mBufferSize){
      mBuffer.pop_front();
    }

    return;
  }

  dvector getVelocity(){
      return {0, 0};
    /*  TODO */
  }
};


#endif
