#ifndef SDLTEXTURE_HPP
#define SDLTEXTURE_HPP

#include "AbstractTexture.hpp"
#include <SDL2/SDL.h>

class SDLTexture : public AbstractTexture{
public:
  SDLTexture(IGraphicsManager* gfx_manager=nullptr, SDL_Texture* =nullptr);
  ~SDLTexture();

  SDL_Texture* getRawTexture(){
    return mpTexture;
  };

protected:
  IGraphicsManager* mpGraphicsManager=nullptr;
  SDL_Texture* mpTexture=nullptr;
};


#endif
