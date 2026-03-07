#include "SDLGraphicsManager.hpp"

#ifndef __EMSCRIPTEN__
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(files);
#endif

#include "engine.hpp"

void
SDLGraphicsManager::renderSplashScreen(){
  const std::lock_guard<std::mutex> lock(mMutex);
  SDL_Rect dst = {(mWindowSize[0] / 2) - 128, (mWindowSize[1] / 2) - 128, 256, 256};
  auto sprite = getSprite("crate.png");
  SDL_RenderCopy(mpRenderer, sprite, NULL, &dst);
  SDL_RenderPresent(mpRenderer);
}


void
SDLGraphicsManager::setDraw(bool on)
{
  if (on == mDraw)
    return;

  else if (on == true) {
    mDraw = true;
    createWindow();
  }

  else {
    mDraw = false;
    destroyBuffers();
    destroyWindow();
  }

  return;
}

void SDLGraphicsManager::destroyWindow(){
  SDL_DestroyWindow(mpWindow);
}


void
SDLGraphicsManager::createWindow(int w, int h)
{

  if(w == -1 || h == -1){
    w = mDefaultWindowSize[0];
    h = mDefaultWindowSize[1];
  }

  mWindowSize = {w, h};

  destroyBuffers();
  {
    const std::lock_guard<std::mutex> lock(mMutex);
    mpWindow = SDL_CreateWindow(mWindowTitle.c_str(),
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             mWindowSize[0],
                             mWindowSize[1],
                             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  }
  resizeWindow(mWindowSize[0], mWindowSize[1]);
  return;
}


void
SDLGraphicsManager::resizeWindow(int x=0, int y=0)
{
  destroyBuffers();
  {
  const std::lock_guard<std::mutex> lock(mMutex);

  if (!mDraw)
    return;

  if (mpWindow) {
    // If no size provided, get it from SDL
    if(x==0 && y==0)
      SDL_GetWindowSize(mpWindow, &x, &y);

    SDL_SetWindowSize(mpWindow, x, y);
  }

  mWindowSize[0] = x;
  mWindowSize[1] = y;

  if(mpRenderer){
    SDL_DestroyRenderer(mpRenderer);
  }
  mpRenderer = SDL_CreateRenderer(mpWindow, -1, mRendererFlags);

  if(mpFrameBuffer){
    SDL_DestroyTexture(mpFrameBuffer);
  }
  mpFrameBuffer = SDL_CreateTexture(mpRenderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    mWindowSize[0],
                                    mWindowSize[1]);

  if(mpNoProcessingBuffer){
    SDL_DestroyTexture(mpNoProcessingBuffer);
  }
  mpNoProcessingBuffer = SDL_CreateTexture(mpRenderer,
                                           SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_TARGET,
                                           mWindowSize[0],
                                           mWindowSize[1]);

  if(mpFrameBuffer){
    SDL_DestroyTexture(mpFrameBuffer);
  }
  mpBloomBuffer = SDL_CreateTexture(mpRenderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    mWindowSize[0],
                                    mWindowSize[1]);

  mScreenRectangle = {0, 0, mWindowSize[0], mWindowSize[1]};
  }
  return;
}


void
SDLGraphicsManager::resetFrameBuffers()
{
  const std::lock_guard<std::mutex> lock(mMutex);

  // Set background colour
  SDL_SetRenderTarget(mpRenderer, mpFrameBuffer);
  SDL_SetRenderDrawColor(mpRenderer, 0x00, 0x10, 0xff, 0xff);
  SDL_RenderFillRect(mpRenderer, nullptr);

  // Clear bloom buffer
  SDL_SetRenderTarget(mpRenderer, mpBloomBuffer);
  SDL_SetTextureBlendMode(mpBloomBuffer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, 0);
  SDL_RenderClear(mpRenderer);

  // Clear no-processing-buffer
  SDL_SetRenderTarget(mpRenderer, mpNoProcessingBuffer);
  SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, 0);
  SDL_RenderClear(mpRenderer);
}


void SDLGraphicsManager::renderClear(){
  const std::lock_guard<std::mutex> lock(mMutex);
  SDL_SetRenderDrawColor(mpRenderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_NONE);
  SDL_RenderClear(mpRenderer);
}


void
SDLGraphicsManager::drawScreen()
{
  renderClear();

  {
  const std::lock_guard<std::mutex> lock(mMutex);

  SDL_RenderCopy(mpRenderer, mpFrameBuffer, nullptr, nullptr);

  // Do bloom and blur

  // Apply brightness effect to window
  if (mBrightness != 0) {
    // Set target and blend mode
    SDL_SetRenderTarget(mpRenderer, nullptr);
    if (mBrightness > 0) {
      // If brightness is positive use additive blending
      SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_ADD);
      SDL_SetRenderDrawColor(mpRenderer, 255, 255, 255, mBrightness);
      SDL_RenderFillRect(mpRenderer, nullptr);
    } else {
      // If brightness is negative draw semi-transparent blac box
      SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, std::abs(mBrightness));
      SDL_RenderFillRect(mpRenderer, nullptr);
    }
  }

  // Draw processing-free buffer
  SDL_SetTextureBlendMode(mpNoProcessingBuffer, SDL_BLENDMODE_BLEND);
  SDL_RenderCopy(mpRenderer, mpNoProcessingBuffer, nullptr, nullptr);

  SDL_RenderPresent(mpRenderer);
  }
  return;
}

/* Lookup the name in our list of sprites and return a pointer to its texture if
 * it exists */
SDL_Texture*
SDLGraphicsManager::getSprite(std::string asset_name)
{
  auto iter = mSpriteList.find(asset_name);

  if(iter == mSpriteList.end()){
    log_message(ERR, "Requested sprite, " + asset_name + " does not exist.");
    return nullptr;
  }
  return iter->second;
}


void
SDLGraphicsManager::loadSpriteFromPath(std::string path){

  #ifdef __ENSCRIPTEN__
  std::string full_path = "assets/" + path;
  SDL_RWops *io = SDL_RWFromFile(full_path.c_str(), "rb");

  #else
  auto fs = cmrc::files::get_filesystem();
  auto dot_pos = path.find('.');
  std::string file_name = path.substr(0, dot_pos);
  std::string file_extension = path.substr(dot_pos);

  auto file = fs.open("files/assets/" + path);
  SDL_RWops *io = SDL_RWFromConstMem(file.begin(), file.end() - file.begin());
  #endif

  SDL_Texture* sprite = IMG_LoadTexture_RW(mpRenderer, io, 1);
  if(sprite){
    mSpriteList[path] = sprite;
    return;
  }

  log_message(ERR, "Failed to load sprite: " + file_name + " " + IMG_GetError() + " Ignoring\n");
}


void
SDLGraphicsManager::blurTexture(SDL_Texture* texture, double size, int passes)
{
  if (size <= 0)
    return;

  if (!texture)
    return;


  // Get width and height of texture
  int width, height;
  SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

  // Create temporary texture for blurring passes
  SDL_Texture* tmpTexture = SDL_CreateTexture(mpRenderer,
                                              SDL_PIXELFORMAT_RGBA8888,
                                              SDL_TEXTUREACCESS_TARGET,
                                              width,
                                              height);

  SDL_SetRenderTarget(mpRenderer, tmpTexture);
  SDL_SetTextureBlendMode(tmpTexture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, 255);
  SDL_RenderClear(mpRenderer);

  // Copy tmpTexture back onto texture
  SDL_SetTextureBlendMode(tmpTexture, SDL_BLENDMODE_NONE);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
  SDL_SetRenderTarget(mpRenderer, tmpTexture);
  SDL_SetTextureAlphaMod(texture, 255);
  SDL_RenderCopy(mpRenderer, texture, nullptr, nullptr);

  // Set appropriate blend modes
  SDL_SetTextureBlendMode(tmpTexture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  // Perform multiple passes of blurring
  SDL_Rect dstRect({ 0, 0, width, height });
  for (int i = 0; i < passes; i++) {
    // Gradually reduce offset size
    int offset = (int)std::max(size - i * size / passes, 1.0);

    // Alternate the direction of the offset to prevent the blur shifting
    // the image
    if (i % 2 == 0) {
      offset *= -1;
    }

    // Copy texture onto tmpTexture shifted horizontally with 50% alpha
    dstRect.x = offset;
    dstRect.y = 0;
    SDL_SetRenderTarget(mpRenderer, tmpTexture);
    SDL_SetTextureAlphaMod(texture, 127);
    SDL_RenderCopy(mpRenderer, texture, nullptr, &dstRect);

    // Copy tmpTexture back onto texture
    SDL_SetRenderTarget(mpRenderer, texture);
    SDL_SetTextureAlphaMod(texture, 255);
    SDL_RenderCopy(mpRenderer, tmpTexture, nullptr, nullptr);

    // Copy texture onto tmpTexture shifted vertically with 50% alpha
    dstRect.x = 0;
    dstRect.y = offset;
    SDL_SetRenderTarget(mpRenderer, tmpTexture);
    SDL_SetTextureAlphaMod(texture, 127);
    SDL_RenderCopy(mpRenderer, texture, nullptr, &dstRect);

    // Copy tmpTexture back onto texture
    SDL_SetRenderTarget(mpRenderer, texture);
    SDL_SetTextureAlphaMod(texture, 255);
    SDL_RenderCopy(mpRenderer, tmpTexture, nullptr, nullptr);
  }

  SDL_DestroyTexture(tmpTexture);
}


// Apply blur to mpBloomBuffer and "add" to frame buffer to create a bloom
void SDLGraphicsManager::applyBloom(double bloom_alpha, double bloom_size,
                                    int passes){
  blurTexture(mpBloomBuffer, bloom_size, passes);
  SDL_SetRenderTarget(mpRenderer, mpFrameBuffer);
  SDL_SetTextureBlendMode(mpBloomBuffer, SDL_BLENDMODE_ADD);
  SDL_SetTextureAlphaMod(mpBloomBuffer, bloom_alpha);
  SDL_RenderCopy(mpRenderer, mpBloomBuffer, nullptr, nullptr);
}

// Apply blur to mpFrameBuffer
void SDLGraphicsManager::applyBlur(double blur_size, int passes){
  blurTexture(mpFrameBuffer, blur_size, passes);
  SDL_SetRenderTarget(mpRenderer, nullptr);
  renderCopy(mpFrameBuffer, nullptr, &mScreenRectangle);
}


void
SDLGraphicsManager::renderCopy(SDL_Texture* texture,
                               SDL_Rect* srcRect,
                               SDL_Rect* dstRect,
                               bool isPostProcessed,
                               int bloomAmount)
{
  const std::lock_guard<std::mutex> lock(mMutex);

  if(!mpRenderer){
    return;
  }

  // Copy the texture onto the appropriate frame buffer
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(mpRenderer, getFrameBuffer(isPostProcessed));
  SDL_RenderCopy(mpRenderer, texture, srcRect, dstRect);

  if (isPostProcessed) {
    // Subtract texture from bloom buffer - this creates the effect that the
    // texture is obscuring the glowing object behind it
    SDL_BlendMode subtractBlendMode =
      SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO,
                                 SDL_BLENDFACTOR_ONE,
                                 SDL_BLENDOPERATION_ADD,
                                 SDL_BLENDFACTOR_ZERO,
                                 SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                 SDL_BLENDOPERATION_ADD);
    SDL_SetTextureBlendMode(texture, subtractBlendMode);
    SDL_SetRenderTarget(mpRenderer, mpBloomBuffer);
    SDL_RenderCopy(mpRenderer, texture, srcRect, dstRect);

    if (bloomAmount > 0) {
      // Add texture to bloom buffer to create a glowing effect
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
      SDL_SetTextureAlphaMod(texture, bloomAmount);
      SDL_RenderCopy(mpRenderer, texture, srcRect, dstRect);
      SDL_SetTextureAlphaMod(texture, 255);
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  }
}


void
SDLGraphicsManager::renderFillRect(std::array<int, 4>& _dstRect,
                                   Uint32 _colour,
                                   bool isPostProcessed,
                                   int bloomAmount)
{
  SDL_Rect dstRect = {(_dstRect[0]),
                      (_dstRect[1]),
                      (_dstRect[2]),
                      (_dstRect[3])};

  // Draw rect to the appropriate frame buffer
  SDL_SetRenderTarget(mpRenderer, getFrameBuffer(isPostProcessed));

  SDL_Color color = {Uint8 (_colour >> 24), Uint8 (_colour >> 16),
                     Uint8 (_colour >> 8), Uint8 (_colour)};
  SDL_SetRenderDrawColor(mpRenderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(mpRenderer, &dstRect);

  if (isPostProcessed) {
    if (bloomAmount > 0) {
      // Add rect to bloom buffer to create a glowing effect
      SDL_SetRenderTarget(mpRenderer, mpBloomBuffer);
      SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(
        mpRenderer, color.r, color.g, color.b, color.a * bloomAmount / 255);
      SDL_RenderFillRect(mpRenderer, &dstRect);
    } else {
      // Subtract rect from bloom buffer - this creates the effect that
      // the rect is obscuring the glowing object behind it
      SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderTarget(mpRenderer, mpBloomBuffer);
      SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, color.a);
      SDL_RenderFillRect(mpRenderer, &dstRect);
    }

    SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
  }
}

void SDLGraphicsManager::drawSprite(std::string asset_name, std::array<int, 4> _dstrect,
                                    bool isPostProcessed, int bloomAmount){
  auto sprite = getSprite(asset_name);
  SDL_Rect dstrect = {_dstrect[0], _dstrect[1], _dstrect[2], _dstrect[3]};
  renderCopy(sprite, nullptr, &dstrect, isPostProcessed=isPostProcessed,
             bloomAmount=bloomAmount);
}

SDLGraphicsManager::SDLGraphicsManager(){
  SDL_Init(SDL_INIT_VIDEO);
}


int SDLGraphicsManager::getWindowFlags(){
  int flags = SDL_GetWindowFlags(mpWindow);
  return flags;
}

bool SDLGraphicsManager::isWindowFullScreen(){
  auto flags = getWindowFlags();

  return flags & SDL_WINDOW_FULLSCREEN_DESKTOP;
}

void SDLGraphicsManager::setWindowFullScreen(bool fullscreen){

  if (fullscreen == isWindowFullScreen())
    return;

  if(fullscreen){
    SDL_SetWindowFullscreen(mpWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
  }

  else{
    SDL_SetWindowFullscreen(mpWindow, 0);
  }

  auto screen_dims = getScreenDimensions();
  handle_system_command({ "resize", std::to_string(screen_dims[0]), std::to_string(screen_dims[1]) });
}


SDLGraphicsManager::~SDLGraphicsManager(){
  destroyBuffers();
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void SDLGraphicsManager::destroyBuffers(){
  if(mpFrameBuffer)
    SDL_DestroyTexture(mpFrameBuffer);

  if(mpNoProcessingBuffer)
    SDL_DestroyTexture(mpNoProcessingBuffer);

  if(mpBloomBuffer)
    SDL_DestroyTexture(mpBloomBuffer);

  if(mpRenderer)
    SDL_DestroyRenderer(mpRenderer);
}

