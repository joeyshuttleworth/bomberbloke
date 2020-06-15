#include "engine.hpp"
#include "Camera.hpp"

level :: level(double x, double y){
  mDimmension[0] = x;
  mDimmension[1] = y;
  // /*The SDL_TEXTUREACCESSTARGET flag allows us to modify this sprite*/
  // // mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,1024, 1024);
  // SDL_SetRenderTarget(_renderer, mpSprite);
  // SDL_SetRenderDrawColor(_renderer, 0, 0xa0, 0xFF, 0xFF);
  // SDL_RenderClear(_renderer);
  // SDL_RenderFillRect(_renderer, NULL);
  // SDL_RenderPresent(_renderer);
  // SDL_SetRenderTarget(_renderer, NULL);
  
    return;
}

level :: ~level(){
  return;
}

void level :: ReloadSprites(){
    for(auto i = mActors.begin(); i != mActors.end(); i++){
    (*i)->ReloadSprite();
  }

  SDL_SetRenderTarget(_renderer, NULL);
  return;
}
std::shared_ptr<actor> level :: GetActor(int id){
  /*search over actors*/
  auto iterator = std::find_if(mActors.begin(), mActors.end(), [&](std::shared_ptr<actor> a) -> bool {return a->GetId() == id;});
  // return the first one (should be unique anyway)
  return *iterator;
}

void level::cleanUp(){
  /* Remove particles with mRemove set! */
  mParticleList.remove_if([](std::shared_ptr<AbstractSpriteHandler> s){return s->ToRemove();});
  /* Now clean up actors */
  mActors.remove_if([](std::shared_ptr<actor> a){return a->mRemove;});
  return;
}


void level :: draw(Camera *cam){
  if(!cam){
    log_message(ERROR, "Attempted to draw with null camera object!");
    return;
  }

  SDL_SetRenderTarget(_renderer, cam->getFrameBuffer());

  SDL_SetRenderDrawColor(_renderer, 0x00, 0x10, 0xff, 0xff);
  SDL_RenderFillRect(_renderer, nullptr);

  //Next draw each actor
  for(auto i = mActors.begin(); i!=mActors.end(); i++){
    (*i)->draw(cam);
  }

  /*  Draw all particles.*/
  for(auto i = mParticleList.begin(); i!= mParticleList.end(); i++){
    /* Remove the previous node if its remove flag is set */
    (*i)->draw(cam);
  }

 return;
}

/* TODO: move all update and movement code into this method  */
void level :: update(){
  for(auto i = mActors.begin(); i != mActors.end(); i++){
    // (*i)->update();
    (*i)->updateSprite();
  }
  return;
}
