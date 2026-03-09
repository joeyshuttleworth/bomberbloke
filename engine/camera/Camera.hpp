#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL.h>
#include <memory>
#include <algorithm>
#include <mutex>
#include "AbstractCamera.hpp"
#include "IGraphicsManager.hpp"
#include "scene.hpp"

class IGraphicsManager;

class Camera : public AbstractCamera{
  public:
  Camera(IGraphicsManager* =nullptr, scene* =nullptr);
  double mZoom = 0.1;
  std::array<double, 2> mPosition = {{ 0, 0 }};


  virtual void init(){};

  virtual ~Camera(){}

  double getZoom(){
    return mZoom;
  }

  std::unique_ptr<AbstractTexture> mpFrameBuffer;

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

  void rumble(double amplitude = 0.02, double timeout = 30);

  std::array<int, 4> getScreenRect(double, double, double, double);

  /**
   * Sets the parameters for the post-processing blur.
   *
   * @param size    Size of the blur, larger is more blury.
   * @param passes  Quality of the blur, larger is higher quality.
   */

  std::array<int, 2> getScreenDimensions(double, double){
    std::array<int, 2> ret_val = {0, 0};

    if(mpGraphicsManager)
      ret_val = mpGraphicsManager->getScreenDimensions();

    return ret_val;
  };

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


protected:

  std::mutex mMutex;

  scene *mpScene;

  const double mMinZoom = 0.01;
  const double mMaxZoom = 50;

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

};


#endif
