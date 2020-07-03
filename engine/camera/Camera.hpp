#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL2/SDL.h>
#include <memory>
#include "scene.hpp"
#include "engine.hpp"

extern bool _halt;
extern SDL_Renderer *_renderer;
extern SDL_Window   *_window;

class Camera{
public:

  Camera(){};

  Camera(std::shared_ptr<scene> lvl){
    mpScene = lvl;
    SetZoom();

    mScreenRectangle.x=0;
    mScreenRectangle.y=0;
    mScreenRectangle.h=mHeight;
    mScreenRectangle.w=mWidth;

    return;
  }

  ~Camera(){
    /* We will get a double free if we destroy the texture after SDL_Quit is called */
    return;
    if(mpFrameBuffer && !_halt){
      SDL_DestroyTexture(mpFrameBuffer);
    }
  };

  SDL_Texture *getFrameBuffer(){
    return mpFrameBuffer;
  }

  double GetZoom(){
    return mZoom;
  }

  void SetZoom(){
    if(!_window){
      mWidth = DEFAULT_WINDOW_HEIGHT;
      mHeight = DEFAULT_WINDOW_WIDTH;
    }
    else
      SDL_GetWindowSize(_window, &mWidth, &mHeight);
    if(mpFrameBuffer)
      SDL_DestroyTexture(mpFrameBuffer);
    mpFrameBuffer = (SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mWidth, mHeight));
    double min = mWidth;
    if(mHeight>min)
      min = mHeight;
    mZoom = ((double)min / mpScene->mDimmension[0]);
    log_message(DEBUG, "Zoom scene is " + std::to_string(mZoom));
    return;
  }

  std::shared_ptr<scene> GetScene(){
    return mpScene;
  }

  void draw(){
    update();
    mScreenRectangle.x = mRumbleOffset[0];
    mScreenRectangle.y = mRumbleOffset[1];
    SDL_SetRenderTarget(_renderer, mpFrameBuffer);
    mpScene->draw(this);
    SDL_SetRenderTarget(_renderer, nullptr);
    SDL_RenderCopy(_renderer, mpFrameBuffer, nullptr, &mScreenRectangle);
    return;
  }

   /* This could be used to make (for example) a FollowCamera class which
   *  inherits from this class
   */
  virtual void update(){
    /*  Rumble effect */
    if(mRumbleTimeout > 0){
      mRumbleTimeout--;
      mRumbleOffset[0] = mRumbleAmplitude*mWidth*std::sin((double)mRumbleTimeout/2);
      mRumbleOffset[1] = mRumbleAmplitude*mHeight*std::cos((double)mRumbleTimeout/5);
    }
    else{
      mRumbleOffset[0] = 0;
      mRumbleOffset[1] = 0;
    }
    return;
  };

  void rumble(double amplitude = 0.02, double timeout = 30);
protected:
  std::shared_ptr<scene> mpScene;
  double mZoom;
  int mWidth;
  int mHeight;
  SDL_Texture *mpFrameBuffer = nullptr;
  SDL_Rect mScreenRectangle;
  std::array<double, 2> mFocusCoordinates = {0,0};
  std::shared_ptr<SDL_Renderer> mpRenderer;
  /*  Use mOffsets for animations on the camera object */
  std::array<double, 2> mOffsets = {0,0};

  /* Variables for rumble animation */
  double mRumbleOffset[2];
  unsigned int mRumbleTimeout = 0;
  /* Maximum displacement in each axis as a proportion of the window size */
  double mRumbleAmplitude;
};
#endif
