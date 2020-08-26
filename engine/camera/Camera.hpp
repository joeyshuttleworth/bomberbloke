#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL2/SDL.h>
#include <memory>
#include "scene.hpp"
#include "engine.hpp"

extern int _window_size[];

extern bool _halt;
extern SDL_Renderer *_renderer;
extern SDL_Window   *_window;

class Camera{
public:

  Camera(){};

  Camera(scene *lvl){
    onResize();

    mScreenRectangle.x=0;
    mScreenRectangle.y=0;
    mScreenRectangle.w= 700;//_window_size[0];
    mScreenRectangle.h= 700;//_window_size[1];

    mpFrameBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mScreenRectangle.w, mScreenRectangle.h);
    return;
  }

  virtual ~Camera(){
    /* We will get a double free if we destroy the texture after SDL_Quit is called */
    return;
    if(mpFrameBuffer && !_halt){
      SDL_DestroyTexture(mpFrameBuffer);
    }
  };

  SDL_Texture *getFrameBuffer(){
    return mpFrameBuffer;
  }

  std::array<int, 2> getScreenDimensions() {
      return {{ mScreenRectangle.w, mScreenRectangle.h }};
  }

  double GetZoom(){
    return mZoom;
  }

  std::shared_ptr<scene> GetScene(){
    return _pScene;
  }

  /**
   * Called by the engine when the window is resized.
   */
  void onResize();

  void draw();

   /* This could be used to make (for example) a FollowCamera class which
   *  inherits from this class
   */
  virtual void update();

  void rumble(double amplitude = 0.02, double timeout = 30);
protected:
  std::shared_ptr<scene> _pScene;
  double mZoom;
  SDL_Texture *mpFrameBuffer = nullptr;
  SDL_Rect mScreenRectangle;
  std::array<double, 2> mFocusCoordinates = {{ 0, 0 }};
  std::shared_ptr<SDL_Renderer> mpRenderer;
  /*  Use mOffsets for animations on the camera object */
  std::array<double, 2> mOffsets = {{ 0, 0 }};

  /* Variables for rumble animation */
  double mRumbleOffset[2];
  unsigned int mRumbleTimeout = 0;
  /* Maximum displacement in each axis as a proportion of the window size */
  double mRumbleAmplitude;
};
#endif
