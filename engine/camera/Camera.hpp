#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "SDL.h"
#include <memory>
#include <algorithm>
#include <mutex>
#include "scene.hpp"

extern int _window_size[];

extern bool _halt;
extern SDL_Renderer *_renderer;
extern SDL_Window   *_window;

class Camera{
  public:
  double mZoom = 0.1;
    std::array<double, 2> mPosition = {{ 0, 0 }};

  Camera(scene *lvl=nullptr){
    mpScene = lvl;

    /* Call this to get screen dimensions */
    onResize();

    mScreenRectangle.x=0;
    mScreenRectangle.y=0;

    /* Different buffers processing effects
     * mpFrameBuffer - current buffer with post processing effects
     * mpNoProcessingBuffer - current buffer with no post processing effects
     * mpBloomBuffer - buffer with blur and "addition" to create a bloom effect.
     * */

    mpFrameBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mScreenRectangle.w, mScreenRectangle.h);
    mpNoProcessingBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mScreenRectangle.w, mScreenRectangle.h);
    mpBloomBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mScreenRectangle.w, mScreenRectangle.h);

    mpScene->updateHudPositions();

    init();
    return;
  }

    virtual void init(){}

  Camera(std::shared_ptr<scene> scn){
      Camera(scn.get());
  }



  virtual ~Camera(){
    /* We will get a double free if we destroy the texture after SDL_Quit is called */
      std::lock_guard<std::mutex> guard{mMutex};
    if(mpFrameBuffer && !_halt){
      SDL_DestroyTexture(mpFrameBuffer);
    }
  }

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

  /**
   * Copies texture onto the appropriate frame buffer.
   *
   * Wrapper for SDL_RenderCopy with additional logic for post-processing
   * effects. See documentation for SDL_RenderCopy for more detail on how
   * srcRect and dstRect affect the rendering.
   *
   * @param texture         Texture to be copied onto buffer.
   * @param srcRect         Rectangle of texture to be copied.
   * @param dstRect         Rectangle of the buffer to copy onto.
   * @param isPostProcessed Set to false to avoid post-processing effects.
   * @param bloomAmount     Determines the amount of bloom applied to texture.
   */
  void renderCopy(SDL_Texture *texture, SDL_Rect *srcRect=nullptr,
      SDL_Rect *dstRect=nullptr, bool isPostProcessed=true, int bloomAmount=0);

  /**
   * Draws a rectangle onto the appropriate frame buffer.
   *
   * Wrapper for SDL_RenderFillRect with additional logic for post-processing
   * effects.
   *
   * @param dstRect         Rectangle of the buffer to draw the rectangle onto.
   * @param colour          Colour of rectangle.
   * @param isPostProcessed Set to false to avoid post-processing effects.
   * @param bloomAmount     Determines the amount of bloom applied to texture.
   */
  void renderFillRect(SDL_Rect *dstRect, SDL_Color colour, bool isPostProcessed=true, int bloomAmount=0);

  std::array<int, 2> getScreenDimensions() {
      return {{ mScreenRectangle.w, mScreenRectangle.h }};
  }

  double GetZoom(){
    return mZoom;
  }

  void SetZoom(double);

  scene* GetScene(){
    return mpScene;
  }

  /**
   * Called by the engine when the window is resized or if mZoom if changed.
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
  void setBlur(double size, int passes=0);

  /**
   * Sets the parameters for the post-processing bloom.
   *
   * @param size    Size of the bloom, larger is more blury.
   * @param alpha   Opacity of the blur, 0-255 where 0 is transparent.
   * @param passes  Quality of the bloom, larger is higher quality.
   */
  void setBloom(double size, int alpha=255, int passes=0);

  /**
   * Sets the parameter for the post-processing brightness effect.
   *
   * @param brightness  Amount of brightness added to the image.
   */
  void setBrightness(int brightness);

  /**
   * Applies a blur effect to a given texture
   *
   * Creates a blur effect by drawing the texture onto itself slightly shifted
   * in one direction and with alpha set to 50%. Performing this repeatedly
   * (passes determines the number of times) in different directions yields an
   * approximation to Gaussian convolution. The size parameter determines the
   * maximum amount the texture is shifted creating a wider blur.
   *
   * @param texture Texture that blur is applied to.
   * @param size    Size of the blur, larger is more blury.
   * @param passes  Quality of the blur, larger is higher quality.
   */
  void blurTexture(SDL_Texture *texture, double size, int passes=8);

  SDL_Rect getScreenRect(double x, double y, double w, double h);

protected:
  std::mutex mMutex;

  scene *mpScene;

  const double mMinZoom = 0.01;
  const double mMaxZoom = 50;

  SDL_Texture *mpFrameBuffer = nullptr;
  SDL_Texture *mpNoProcessingBuffer = nullptr;
  SDL_Texture *mpBloomBuffer = nullptr;
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

  // Determines the size of the bloom, larger is blurier.
  double mBloomSize = 4;
  // Determines the opacity of the bloom, 0-255 where 0 is completely transparent.
  double mBloomAlpha = 255;
  // Determines the quality of the bloom, larger is better.
  int mBloomPasses = 4;

  // Controls how bright or dark the image is - ranges from -255 to 255.
  int mBrightness = 0;
};


#endif
