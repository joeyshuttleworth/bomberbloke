#ifndef SDLGRAPHICSMANAGER_HPP
#define SDLGRAPHICSMANAGER_HPP

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "IGraphicsManager.hpp"

using SpriteList =  std::vector<std::pair<std::string, SDL_Texture*>>;

class SDLGraphicsManager : IGraphicsManager{
protected:
  SDL_Texture* getSprite(std::string);
  void loadSprites();
  void createWindow();

  bool mDraw = true;
  SDL_Renderer* mpRenderer;
  SDL_Window* mpWindow;
  SDL_Texture* mpNoProcessingBuffer;
  SDL_Texture* mpFrameBuffer;
  SDL_Texture* mpBloomBuffer;
  SpriteList mSpriteList;
  int mWindowSize[2] = {0, 0};
  std::string mWindowTitle = "Bomberbloke";
  scene mpScene;
  std::mutex mMutex;

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
  void renderCopy(SDL_Texture *texture, SDL_Rect *srcRect=nullptr,
                       SDL_Rect *dstRect=nullptr, bool isPostProcessed=true, int bloomAmount=0);

  SDL_Rect getScreenRect(double x, double y, double w, double h);

public:
  // Allows for SDL like function calls
   void renderCopy() override;

  void resizeWindow(int w, int h) override;

  void init() override;

  void renderSplashScreen() override;

  void setDraw(bool on) override;

  void refreshSprites() override;

  void drawScreen() override;

  void loadSpriteFromPath(std::string) override;

  void resetFrameBuffers() override;

  void blurTexture(SDL_Texture*, double, int) override;

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
    return {{ mWindowSize[0], mWindowSize[1] }};
  }


};


#endif
