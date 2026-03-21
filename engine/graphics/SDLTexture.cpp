#include "SDLTexture.hpp"

SDLTexture::SDLTexture(IGraphicsManager* gfx_manager, SDL_Texture *tex) :
  mpGraphicsManager(gfx_manager),
  mpTexture(tex)
{
}

SDLTexture::~SDLTexture(){
  SDL_DestroyTexture(mpTexture);
}


