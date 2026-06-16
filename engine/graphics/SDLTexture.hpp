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

  void setRawTexture(SDL_Texture* tex){
    mpTexture = tex;
  }

  std::array<int, 2> getDimensions() override{
    int w, h;
    SDL_QueryTexture(mpTexture, NULL, NULL, &w, &h);

    return std::array<int, 2>{w, h};
  }

protected:
  IGraphicsManager* mpGraphicsManager=nullptr;
  SDL_Texture* mpTexture=nullptr;
};


#endif
