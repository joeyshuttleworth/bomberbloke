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
    mpScene = lvl;

    /* Call this to get screen dimensions */
    onResize();

    mScreenRectangle.x=0;
    mScreenRectangle.y=0;

    mpFrameBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mScreenRectangle.w, mScreenRectangle.h);
    mpNoProcessingBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mScreenRectangle.w, mScreenRectangle.h);

    mpScene->updateHudPositions();
    return;
  }

  virtual ~Camera(){
    /* We will get a double free if we destroy the texture after SDL_Quit is called */
    return;
    if(mpFrameBuffer && !_halt){
      SDL_DestroyTexture(mpFrameBuffer);
    }
  };

  /**
   * Returns frame buffer for drawing to camera.
   *
   * @param isPostProcessed Setting this to true returns a buffer that post
   *                        processing effects are applied to.
   */
  SDL_Texture *getFrameBuffer(bool isPostProcessed=true){
    if (isPostProcessed)
      return mpFrameBuffer;
    else
      return mpNoProcessingBuffer;
  }

  void resetFrameBuffer();

  std::array<int, 2> getScreenDimensions() {
      return {{ mScreenRectangle.w, mScreenRectangle.h }};
  }

  double GetZoom(){
    return mZoom;
  }

  scene* GetScene(){
    return mpScene;
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

  /**
   * Sets the parameters for the post-processing blur.
   *
   * @param size    Size of the blur, larger is more blury.
   * @param passes  Quality of the blur, larger is higher quality.
   */
  void blur(double size, int passes=8);

  /**
   * Applies a blur effect to a given texture
   *
   * @param texture Texture that blur is applied to.
   * @param size    Size of the blur, larger is more blury.
   * @param passes  Quality of the blur, larger is higher quality.
   */
  void blurTexture(SDL_Texture *texture, double size, int passes=8);

protected:
  scene *mpScene;
  double mZoom;
  SDL_Texture *mpFrameBuffer = nullptr;
  SDL_Texture *mpNoProcessingBuffer = nullptr;
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

  // Determines the size of the blur, larger is blurier.
  double mBlurSize = 0;
  // Determines the quality of the blur, larger is better.
  int mBlurPasses = 8;
};
#endif
