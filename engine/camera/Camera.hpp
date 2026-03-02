#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL.h>
#include <memory>
#include <algorithm>
#include "scene.hpp"
#include "AbstractCamera.hpp"

class scene;
class IGraphicsInterface;

class Camera : public AbstractCamera{
  public:
  double mZoom = 0.1;
  std::array<double, 2> mPosition = {{ 0, 0 }};

  Camera(IGraphicsInterface*, scene*);

  virtual void init(){};

  virtual ~Camera(){}

  double getZoom(){
    return mZoom;
  }

  void setZoom(double);

  /**
   * Called by the engine when the window is resized or if mZoom if changed.
   */
  void onResize();

  void draw();

   /* Used to make (for example) a FollowCamera class which
   *  inherits from this class
   */
  virtual void update();

protected:


  void resetFrameBuffer();


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

  std::mutex mMutex;

  scene *mpScene;

  const double mMinZoom = 0.01;
  const double mMaxZoom = 50;

  SDL_Texture *mpFrameBuffer = nullptr;
  SDL_Texture *mpNoProcessingBuffer = nullptr;
  SDL_Texture *mpBloomBuffer = nullptr;
  SDL_Rect mScreenRectangle;
  std::array<double, 2> mFocusCoordinates = {{ 0, 0 }};
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

  IGraphicsInterface* mpGraphicsInterface;
};


#endif
