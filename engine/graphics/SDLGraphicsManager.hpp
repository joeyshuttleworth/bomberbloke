#ifndef SDLGRAPHICSMANAGER_HPP
#define SDLGRAPHICSMANAGER_HPP

#include <SDL.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "IGraphicsManager.hpp"
#include "SDLTexture.hpp"


struct SDLFontResoruce{
  TTF_Font* font;
  std::vector<unsigned char> data;
};

using SpriteList =  std::map<std::string, std::unique_ptr<SDLTexture>>;
class scene;

class SDLGraphicsManager : public IGraphicsManager{
protected:
  AbstractTexture* getSprite(std::string) override;

  SDL_Texture* createRawTexture(int width, int height, bool clear);

  const Uint32 mRendererFlags = SDL_RENDERER_TARGETTEXTURE;
  const std::array<int, 2> mDefaultWindowSize = {600, 800};

  std::map<SDLTexture*, std::unique_ptr<SDLTexture>> mTextures;

  bool mDraw = true;
  SDL_Renderer* mpRenderer;
  SDL_Window* mpWindow;
  SDLTexture* mpFrameBuffer;

  std::array<SDLTexture*, 2> mpBlurBuffers;

  SpriteList mSpriteList;
  std::array<int, 2> mWindowSize = {0, 0};
  std::string mWindowTitle = "Bomberbloke";
  scene* mpScene;
  std::mutex mMutex;
  int mBrightness = 0;

  std::map<std::string, SDLFontResoruce> mFonts = {};

  Rect mScreenRectangle = {0, 0, 0, 0};

  /**
   * Returns frame buffer for drawing to camera.
   *
   * @param isPostProcessed Setting this to true returns a buffer that post
   *                        processing effects are applied to.
   */
  AbstractTexture* getFrameBuffer(){
    return (AbstractTexture*) mpFrameBuffer;
  }

  void resetFrameBuffer();

  /**
   * Sets the parameter for the post-processing brightness effect.
   *
   * @param brightness  Amount of brightness added to the image.
   */
  void setBrightness(int brightness) override {mBrightness = brightness;};

  void destroyBuffers();


  void renderCopy(SDL_Texture* texture, Rect* srcRect=nullptr,
                  Rect* dstRect=nullptr,
                  SDL_Texture* target=nullptr);

public:

  void renderClear(AbstractTexture*) override;

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
  void renderCopy(AbstractTexture* texture, Rect* srcRect=nullptr,
                  Rect* dstRect=nullptr,
                  AbstractTexture* target=nullptr) override;

  // Allows for SDL like function calls
  void resizeWindow(int, int) override;

  void renderSplashScreen() override;

  void setDraw(bool on) override;

  void drawScreen() override;

  void loadSpriteFromPath(std::string) override;

  void resetFrameBuffers() override;

  virtual AbstractTexture* renderSolidText(std::string, int, std::string, uint32_t, AbstractTexture*) override;

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
 void blurTexture(SDL_Texture* texture, double size, int passes);

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
  void renderFillRect(std::array<int, 4>&, uint32_t,
                      AbstractTexture* = nullptr) override;

  std::array<int, 2> getScreenDimensions() override{
    return {{ mWindowSize[0], mWindowSize[1] }};
  }

  void drawSprite(std::string, std::array<int, 4>, AbstractTexture* =nullptr) override;

  void applyBloom(double, double, int, AbstractTexture* =nullptr, Rect* = nullptr,
                  AbstractTexture* =nullptr) override;
  void applyBlur(double, int, AbstractTexture*) override;

  void createWindow(int=-1, int=-1) override;

  int getWindowFlags();

  void setWindowFullScreen(bool) override;

  bool isWindowFullScreen() override;

  void destroyWindow() override;

  void init() override;

  AbstractTexture* createTexture(int=0, int=0) override;

  void destroyTexture(AbstractTexture*) override;

  void loadFont(std::string, int) override;

  TTF_Font* getFont(std::string, int);

  std::array<int, 2> sizeText(std::string, int, std::string) override;

  std::shared_ptr<Text> createText(std::string, std::string, int) override;

  const SDL_BlendMode subtractBlendMode =
    SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO,
                               SDL_BLENDFACTOR_ONE,
                               SDL_BLENDOPERATION_ADD,
                               SDL_BLENDFACTOR_ZERO,
                               SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                               SDL_BLENDOPERATION_ADD);

  SDLGraphicsManager();
  virtual ~SDLGraphicsManager();

};

#endif
