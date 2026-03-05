#ifndef SDLGRAPHICSMANAGER_HPP
#define SDLGRAPHICSMANAGER_HPP

#include <SDL.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <SDL_image.h>

#include "IGraphicsManager.hpp"

using SpriteList =  std::map<std::string, SDL_Texture*>;
class scene;

class SDLGraphicsManager : public IGraphicsManager{
protected:
  SDL_Texture* getSprite(std::string);

  bool mDraw = true;
  SDL_Renderer* mpRenderer;
  SDL_Window* mpWindow;
  SDL_Texture* mpNoProcessingBuffer;
  SDL_Texture* mpFrameBuffer;
  SDL_Texture* mpBloomBuffer;
  SpriteList mSpriteList;
  int mWindowSize[2] = {0, 0};
  std::string mWindowTitle = "Bomberbloke";
  scene* mpScene;
  std::mutex mMutex;
  int mBrightness = 0;

  SDL_Rect mScreenRectangle = {0, 0, 0, 0};

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

 void setBlur(double size, int passes=0) override;

  /**
   * Sets the parameters for the post-processing bloom.
   *
   * @param size    Size of the bloom, larger is more blury.
   * @param alpha   Opacity of the blur, 0-255 where 0 is transparent.
   * @param passes  Quality of the bloom, larger is higher quality.
   */
  void setBloom(double size, int alpha=255, int passes=0) override;

  /**
   * Sets the parameter for the post-processing brightness effect.
   *
   * @param brightness  Amount of brightness added to the image.
   */
  void setBrightness(int brightness) override;

  void renderClear() override;

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

public:

  void renderCopy(SDL_Texture *texture, SDL_Rect *srcRect=nullptr,
                  SDL_Rect *dstRect=nullptr, bool isPostProcessed=true, int bloomAmount=0);

  // Allows for SDL like function calls
  void resizeWindow(int, int) override;

  void init() override;

  void renderSplashScreen() override;

  void setDraw(bool on) override;

  void drawScreen() override;

  void loadSpriteFromPath(std::string) override;

  void resetFrameBuffers() override;

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
  void blurTexture(SDL_Texture*, double, int);

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
  void renderFillRect(std::array<int, 4>&, Uint32,
                      bool isPostProcessed=true, int bloomAmount=0) override;

  std::array<int, 2> getScreenDimensions() override{
    return {{ mWindowSize[0], mWindowSize[1] }};
  }

  void drawSprite(std::string, std::array<int, 4>, bool=true, int=0) override;

  void applyBloom(double, double, int) override;
  void applyBlur(double, int) override;

  void createWindow() override;

  SDLGraphicsManager();
  ~SDLGraphicsManager();

};


#endif
