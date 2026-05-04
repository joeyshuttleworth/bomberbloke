#include <sstream>
#include <SDL2/SDL_ttf.h>

#include "SDLGraphicsManager.hpp"
#include "SDLTexture.hpp"
#include "IOSystem.hpp"
#include "Text.hpp"

#ifndef __EMSCRIPTEN__
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(files);
#endif

#include "engine.hpp"
#include "Text.hpp"


void
SDLGraphicsManager::renderSplashScreen(){
  const std::lock_guard<std::mutex> lock(mMutex);
  SDL_Rect dst = {(mWindowSize[0] / 2) - 128, (mWindowSize[1] / 2) - 128, 256, 256};
  auto sprite = (SDLTexture*) getSprite("crate.png");
  if(sprite){
    SDL_RenderCopy(mpRenderer, sprite->getRawTexture(), nullptr, &dst);
  }

  SDL_RenderPresent(mpRenderer);

}


AbstractTexture*
SDLGraphicsManager::renderSolidText(std::string font_name, int font_size, std::string text, uint32_t colour, AbstractTexture* target){

  if(text=="")
    return nullptr;

  // Render to surface
  TTF_Font* font = getFont(font_name, font_size);

  SDL_Color sdl_colour = {(Uint8) (colour >> 24), (Uint8) (colour >> 16), (Uint8) (colour >> 8),
                         (Uint8) colour};

  SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), sdl_colour);
  if (!surface) {
    std::ostringstream e_msg;
    e_msg << "Unable to render text surface: " << TTF_GetError();
    log_message(WARNING, e_msg.str());
    return nullptr;
  }

  // Create texture from surface
  SDL_Texture* raw_texture = SDL_CreateTextureFromSurface(mpRenderer, surface);

  std::ostringstream e_msg;
  e_msg << "Unable to create text texture: " << TTF_GetError();
  if (!raw_texture) {
    log_message(WARNING, e_msg.str());
  }

  SDLTexture *raw_ptr = nullptr;
  if(!target){
    std::unique_ptr<SDLTexture> texture = std::make_unique<SDLTexture>(this, raw_texture);
    raw_ptr = texture.get();
    mTextures[raw_ptr] = std::move(texture);
  }
  else{
    auto raw_target_texture = ((SDLTexture*) target)->getRawTexture();
    SDL_SetRenderTarget(mpRenderer, raw_target_texture);
    SDL_RenderCopy(mpRenderer, raw_texture, nullptr, nullptr);
    SDL_SetRenderTarget(mpRenderer, raw_target_texture);
    raw_ptr = (SDLTexture*) target;
  }

  // Cleanup
  SDL_FreeSurface(surface);

  return (AbstractTexture*) raw_ptr;
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
SDLGraphicsManager::resizeWindow(int x=-1, int y=-1)
{
  destroyBuffers();
  {
  const std::lock_guard<std::mutex> lock(mMutex);

  if (!mDraw)
    return;

  if (mpWindow) {
    // If no size provided, get it from SDL
    if(x<=0 || y<=0)
      SDL_GetWindowSize(mpWindow, &x, &y);

    else
      SDL_SetWindowSize(mpWindow, x, y);
  }

  mWindowSize[0] = x;
  mWindowSize[1] = y;

  if(!mpRenderer){
    mpRenderer = SDL_CreateRenderer(mpWindow, -1, mRendererFlags);
  }
  SDL_RenderSetViewport(mpRenderer, NULL);

  // mpRenderer = SDL_CreateRenderer(mpWindow, -1, mRendererFlags);

  mpFrameBuffer = (SDLTexture*) createTexture(mWindowSize[0], mWindowSize[1]);
  mpBloomBuffer = (SDLTexture*) createTexture(mWindowSize[0], mWindowSize[1]);

  mScreenRectangle = {0, 0, mWindowSize[0], mWindowSize[1]};
  }
  return;
}


void
SDLGraphicsManager::resetFrameBuffers()
{
  const std::lock_guard<std::mutex> lock(mMutex);

  if(mpFrameBuffer){
    // Set background colour
    SDL_SetRenderTarget(mpRenderer, mpFrameBuffer->getRawTexture());
    SDL_SetRenderDrawColor(mpRenderer, 0x00, 0x10, 0xff, 0xff);
    SDL_RenderClear(mpRenderer);
    SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
  }

  if(mpBloomBuffer){
    SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(mpRenderer, mpBloomBuffer->getRawTexture());
    SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, 0);
    SDL_RenderClear(mpRenderer);
    SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
  }

  SDL_SetRenderTarget(mpRenderer, nullptr);
}


void SDLGraphicsManager::renderClear(AbstractTexture* target){
  if(!mpFrameBuffer)
    return;

  auto _target = mpFrameBuffer->getRawTexture();

  if(target)
    _target = ((SDLTexture*) target)->getRawTexture();

  SDL_SetRenderTarget(mpRenderer, _target);
  SDL_SetRenderDrawColor(mpRenderer, 0x00, 0x00, 0x00, 0x00);
  SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_NONE);
  SDL_RenderClear(mpRenderer);

  SDL_SetRenderTarget(mpRenderer, nullptr);
  SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
}


void
SDLGraphicsManager::drawScreen()
{
  {
  const std::lock_guard<std::mutex> lock(mMutex);

  if(!mpFrameBuffer)
    return;

  if(!mpRenderer)
    return;

  if(!mDraw)
    return;

  // Clear screen
  SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderTarget(mpRenderer, nullptr);
  SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, 0);
  SDL_RenderClear(mpRenderer);

  // Draw main framebuffer
  SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_NONE);
  SDL_RenderCopy(mpRenderer, mpFrameBuffer->getRawTexture(), nullptr, nullptr);

  // Draw bloom buffer
  // SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
  // SDL_SetRenderTarget(mpRenderer, nullptr);
  // SDL_RenderCopy(mpRenderer, mpBloomBuffer->getRawTexture(), nullptr, nullptr);

  // // Apply brightness effect to window
  // if (mBrightness != 0) {
  //   // Set target and blend mode
  //   SDL_SetRenderTarget(mpRenderer, nullptr);
  //   if (mBrightness > 0) {
  //     // If brightness is positive use additive blending
  //     SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_ADD);
  //     SDL_SetRenderDrawColor(mpRenderer, 255, 255, 255, mBrightness);
  //     SDL_RenderFillRect(mpRenderer, nullptr);
  //   } else {
  //     // If brightness is negative draw semi-transparent black box
  //     SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
  //     SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, std::abs(mBrightness));
  //     SDL_RenderFillRect(mpRenderer, nullptr);
  //   }
  // }

  SDL_RenderPresent(mpRenderer);
  }
  return;
}

/* Lookup the name in our list of sprites and return a pointer to its texture if
 * it exists */
AbstractTexture*
SDLGraphicsManager::getSprite(std::string asset_name)
{
  auto iter = mSpriteList.find(asset_name);

  if(iter == mSpriteList.end()){
    log_message(ERR, "Requested sprite, " + asset_name + " does not exist.");
    return nullptr;
  }

  return (AbstractTexture*) iter->second.get();
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

  SDL_Texture* _sprite = IMG_LoadTexture_RW(mpRenderer, io, 1);
  if(_sprite){
    mSpriteList[path] = std::make_unique<SDLTexture>(this, _sprite);
    return;
  }

  log_message(ERR, "Failed to load sprite: " + file_name + " " + IMG_GetError() + " Ignoring\n");
}


void
SDLGraphicsManager::blurTexture(SDL_Texture* texture, double size, int passes, SDL_Texture* target)
{
  if (size <= 0)
    return;

  if (!texture)
    return;

  if(!target){
    target = texture;
  }
  else{
    renderCopy(texture, nullptr, nullptr, 0, target);
  }

  // Get width and height of texture
  int width, height;
  SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

  // TODO avoid recreating this texture every time. Just use something like
  // mpBlurBuffer
  //Create temporary texture for blurring passes
  SDL_Texture* tmpTexture = SDL_CreateTexture(mpRenderer,
                                              SDL_PIXELFORMAT_RGBA8888,
                                              SDL_TEXTUREACCESS_TARGET,
                                              width,
                                              height);

  SDL_SetRenderTarget(mpRenderer, tmpTexture);
  SDL_SetTextureBlendMode(tmpTexture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, 0);
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
    SDL_SetTextureAlphaMod(tmpTexture, 127);
    SDL_RenderCopy(mpRenderer, tmpTexture, nullptr, &dstRect);

    // Copy tmpTexture back onto texture
    SDL_SetRenderTarget(mpRenderer, tmpTexture);
    SDL_SetTextureAlphaMod(tmpTexture, 255);
    SDL_RenderCopy(mpRenderer, tmpTexture, nullptr, nullptr);

    // Copy texture onto tmpTexture shifted vertically with 50% alpha
    dstRect.x = 0;
    dstRect.y = offset;
    SDL_SetRenderTarget(mpRenderer, tmpTexture);
    SDL_SetTextureAlphaMod(tmpTexture, 127);
    SDL_RenderCopy(mpRenderer, tmpTexture, nullptr, &dstRect);

    // Copy tmpTexture back onto texture
    SDL_SetRenderTarget(mpRenderer, tmpTexture);
    SDL_SetTextureAlphaMod(tmpTexture, 255);
    SDL_RenderCopy(mpRenderer, tmpTexture, nullptr, nullptr);
  }

  SDL_SetRenderTarget(mpRenderer, target);
  SDL_RenderCopy(mpRenderer, tmpTexture, nullptr, nullptr);
  SDL_SetRenderTarget(mpRenderer, nullptr);

  SDL_DestroyTexture(tmpTexture);
}


void SDLGraphicsManager::applyBloom(double bloom_alpha, double bloom_size,
                                    int passes, AbstractTexture* _texture,
                                    Rect* dstRect,
                                    AbstractTexture* _target){

  if(passes == 0)
    return;

  if(bloom_alpha * bloom_size == 0)
    return;

  auto target = (SDLTexture*) _target;
  if(!target)
    target = mpBloomBuffer;

  auto texture = (SDLTexture*) _texture;

  if(!texture)
    texture = mpFrameBuffer;

  Uint8 orig_alpha = 0;
  SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);

  SDL_SetTextureBlendMode(texture->getRawTexture(), SDL_BLENDMODE_BLEND);
  SDL_GetTextureAlphaMod(texture->getRawTexture(), &orig_alpha);
  SDL_SetTextureAlphaMod(texture->getRawTexture(), bloom_alpha / 256);

  SDL_SetRenderTarget(mpRenderer, target->getRawTexture());

  if(dstRect){
    SDL_Rect _dstRect = {(*dstRect)[0], (*dstRect)[1], (*dstRect)[2], (*dstRect)[3]};
    SDL_RenderCopy(mpRenderer, texture->getRawTexture(), nullptr, &_dstRect);
  }
  else{
    SDL_RenderCopy(mpRenderer, texture->getRawTexture(), nullptr, nullptr);
  }

  SDL_SetTextureAlphaMod(texture->getRawTexture(), orig_alpha);
  SDL_SetRenderTarget(mpRenderer, nullptr);
}

// Apply blur to mpFrameBuffer
void SDLGraphicsManager::applyBlur(double blur_size, int passes, AbstractTexture* _target){
  auto target = (SDLTexture*) _target;

  if(!target)
    target = mpFrameBuffer;

  blurTexture(target->getRawTexture(), blur_size, passes);
}

void SDLGraphicsManager::renderCopy(AbstractTexture* texture,
                                    Rect *_srcRect,
                                    Rect *_dstRect,
                                    double bloomAmount,
                                    AbstractTexture* target
                                    ){


  SDL_Texture* _texture = texture ? ((SDLTexture*) texture)->getRawTexture() : nullptr;
  SDL_Texture* _target = target ? ((SDLTexture*) target)->getRawTexture() : nullptr;

  renderCopy(_texture, _srcRect, _dstRect, bloomAmount, _target);
}

void
SDLGraphicsManager::renderCopy(SDL_Texture* texture,
                               Rect* _srcRect,
                               Rect* _dstRect,
                               double bloomAmount,
                               SDL_Texture* target,
                               bool occlude_bloom)
{
  SDL_Rect *srcRect = new SDL_Rect{0, 0, 0, 0};
  SDL_Rect *dstRect = new SDL_Rect{0, 0, 0, 0};

  if(_srcRect)
    *srcRect = SDL_Rect{(*_srcRect)[0], (*_srcRect)[1], (*_srcRect)[2], (*_srcRect)[3]};
  if(_dstRect)
    *dstRect = SDL_Rect{(*_dstRect)[0], (*_dstRect)[1], (*_dstRect)[2], (*_dstRect)[3]};

  if(srcRect->w == 0 && srcRect->h == 0)
    srcRect = nullptr;

  if(dstRect->w == 0 && dstRect->h == 0)
    dstRect = nullptr;

  if(!target && mpFrameBuffer)
    target = mpFrameBuffer->getRawTexture();

  // Copy the texture onto the appropriate frame buffer
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  if(bloomAmount > 0)
  {
    // Necessary because applyBloom takes AbstractTexture*
    // Consider implementing a private raw SDL_Texture version
    auto _texture = SDLTexture{this, texture};
    auto _target = SDLTexture{this, target};

    applyBloom(bloomAmount, bloomAmount, 1, &_texture, _dstRect, mpBloomBuffer);

    // A bit hacky - ensure that SDL_DestoryTexture isn't called on these raw
    // textures that we don't own
    _texture.setRawTexture(nullptr);
    _target.setRawTexture(nullptr);
  }

  if(occlude_bloom){
    // SDL_SetRenderTarget(mpRenderer, mpBloomBuffer->getRawTexture());
    // SDL_SetTextureBlendMode(texture, subtractBlendMode);
    // SDL_RenderCopy(mpRenderer, texture, nullptr, dstRect);
    // SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  }

  SDL_SetRenderTarget(mpRenderer, target);
  SDL_RenderCopy(mpRenderer, texture, srcRect, dstRect);

  delete srcRect;
  delete dstRect;

  SDL_SetRenderTarget(mpRenderer, nullptr);
}

void
SDLGraphicsManager::renderFillRect(std::array<int, 4>& _dstRect,
                                   Uint32 _colour, double bloomAmount,
                                   AbstractTexture* _target, bool occlude_bloom)
{
  SDL_Rect dstRect = {(_dstRect[0]),
                      (_dstRect[1]),
                      (_dstRect[2]),
                      (_dstRect[3])};

  auto target = (SDLTexture*) _target;
  // Draw rect to the appropriate frame buffer
  if(!target){
    auto _fb = (SDLTexture*) mpFrameBuffer;
    target = _fb;
  }


  SDL_SetRenderTarget(mpRenderer, target->getRawTexture());
  SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
  SDL_Color color = {Uint8 (_colour >> 24), Uint8 (_colour >> 16),
                     Uint8 (_colour >> 8), Uint8 (_colour)};
  SDL_SetRenderDrawColor(mpRenderer, color.r, color.g, color.b, color.a);
  SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
  SDL_RenderFillRect(mpRenderer, &dstRect);

  SDL_SetRenderDrawColor(mpRenderer,
                         color.r, color.g, color.b, int((color.a * bloomAmount) / 256));

  if(occlude_bloom){
    // Subtract from bloom buffer
    // SDL_SetTextureBlendMode(mpBloomBuffer->getRawTexture(), subtractBlendMode);
    // SDL_SetRenderTarget(mpRenderer, mpBloomBuffer->getRawTexture());
    // SDL_RenderFillRect(mpRenderer, &dstRect);
    // SDL_SetTextureBlendMode(mpBloomBuffer->getRawTexture(), SDL_BLENDMODE_BLEND);
  }

  SDL_SetRenderTarget(mpRenderer, nullptr);
}

void SDLGraphicsManager::drawSprite(std::string asset_name, Rect _dstrect,
                                    double bloomAmount, AbstractTexture* _target){
  auto sprite = (SDLTexture*) getSprite(asset_name);

  if(!sprite)
    return;

  SDL_Texture* target = nullptr;
  if(_target)
    target = ((SDLTexture*) _target)->getRawTexture();

  renderCopy(sprite->getRawTexture(), nullptr, &_dstrect,
             bloomAmount, target);
}


SDLGraphicsManager::SDLGraphicsManager(){
}

void SDLGraphicsManager::init(){
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

  // Update member variables for new screen size
  resizeWindow(-1, -1);
}


SDLGraphicsManager::~SDLGraphicsManager(){
  destroyBuffers();

  // Unload fonts
  for(auto font : mFonts){
    TTF_CloseFont(font.second.font);
  }

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void
SDLGraphicsManager::loadFont(std::string path, int size){
  // TODO Add error checking
  SDLFontResoruce res;
#ifdef __ENSCRIPTEN__
  std::string full_path = "assets/" + path + ".ttf";
  res.font = TTF_OpenFont(full_path.c_str(), size);
#else
  std::string full_path = "files/assets/" + path + ".ttf";
  try{
  auto fs = cmrc::files::get_filesystem();
  auto file = fs.open(full_path);

  std::vector<char> buffer(file.begin(), file.end());
  res.data = std::vector<unsigned char>(file.begin(), file.end());

  SDL_RWops* rw = SDL_RWFromConstMem(res.data.data(), res.data.end() - res.data.begin());
  res.font = TTF_OpenFontRW(rw, 1, size);
  }
  catch (const std::exception& e){
    std::ostringstream err;
    err << "Couldn't open font file " << full_path;
    log_message(ERR, err.str());
    return;
  }
#endif
  std::ostringstream font_name;
  font_name <<  path << "_" << size;
  mFonts[font_name.str()] = std::move(res);
}

TTF_Font* SDLGraphicsManager::getFont(std::string path, int size){
  // First check if font in mFonts
  std::ostringstream font_name;
  font_name << path << "_" << size;

  if(!mFonts.contains(font_name.str())){
    // Font not loaded in the right size, load it first
    loadFont(path, size);
  }

  if(mFonts.contains(font_name.str()))
    return mFonts[font_name.str()].font;

  return nullptr;
}

void SDLGraphicsManager::destroyBuffers(){
  destroyTexture(mpFrameBuffer);
  destroyTexture(mpBloomBuffer);
}


SDL_Texture* SDLGraphicsManager::createRawTexture(int width, int height, bool clear){
  SDL_Texture* texture =  SDL_CreateTexture(mpRenderer,
                                            SDL_PIXELFORMAT_RGBA8888,
                                            SDL_TEXTUREACCESS_TARGET,
                                            width,
                                            height);

  if(clear){
    SDL_SetRenderTarget(mpRenderer, texture);
    SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, 0);
    SDL_RenderClear(mpRenderer);
    SDL_SetRenderTarget(mpRenderer, nullptr);
  }

  return texture;
}

AbstractTexture* SDLGraphicsManager::createTexture(int width, int height){
  auto tex = createRawTexture(width, height, true);
  std::unique_ptr<SDLTexture> tex_ptr = std::make_unique<SDLTexture>(this, tex);
  auto raw_ptr = tex_ptr.get();
  mTextures[raw_ptr] = std::move(tex_ptr);

  return raw_ptr;
}


void SDLGraphicsManager::destroyTexture(AbstractTexture* _texture){
  SDLTexture* tex = (SDLTexture*) _texture;
  if(mTextures.contains(tex)){
    mTextures.erase(tex);
  }
}

std::array<int, 2> SDLGraphicsManager::sizeText(std::string font, int font_size,
                                                std::string render_text){
  int w = 0;
  int h = 0;

  TTF_SizeText(getFont(font, font_size), render_text.c_str(), &w, &h);

  return std::array<int, 2>{w, h};
}

std::shared_ptr<Text> SDLGraphicsManager::createText(std::string render_text, std::string font, int font_size){
  return std::make_shared<Text>(this, font, font_size, render_text);
}
