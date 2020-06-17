#include "engine.hpp"

level :: level(){
  mDimmension[0] = 10;
  mDimmension[1] = 10;
  /*The SDL_TEXTUREACCESSTARGET flag allows us to modify this sprite*/
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,1024, 1024);
  SDL_SetRenderTarget(_renderer, mpSprite);
  SDL_SetRenderDrawColor(_renderer, 0, 0xa0, 0xFF, 0xFF);
  SDL_RenderClear(_renderer);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_RenderPresent(_renderer);
  SDL_SetRenderTarget(_renderer, NULL);
  return;
}

level :: ~level(){
  return;
}

level :: level(const level &lvl){
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1024, 1024);
  memcpy(mpSprite, lvl.mpSprite, sizeof(SDL_Surface));
}

void level :: ReloadSprites(){
  if(mpSprite)
    SDL_DestroyTexture(mpSprite);
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,1024, 1024);
  SDL_SetRenderTarget(_renderer, mpSprite);
  SDL_SetRenderDrawColor(_renderer, 0, 0xa0, 0xFF, 0xFF);
  SDL_RenderClear(_renderer);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_RenderPresent(_renderer);

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


void level::handleMovement(){
  /*Iterate over all moving actors*/
    for (auto i = _level.mActors.begin(); i != _level.mActors.end(); i++) {
        bool collision = false;
        std::array<double,4> seperation_vectors;
        /*Update actors*/

        (*i)->update();

        if ((*i)->is_moving()) {
            (*i)->move((*i)->mPosition[0] + (*i)->mVelocity[0], (*i)->mPosition[1] + (*i)->mVelocity[1]);
            if ((*i)->mCollides == true) {
                /*Check for collisions*/
                for (auto j = _level.mActors.begin(); j != _level.mActors.end(); j++) {
                    if (i == j)
                        continue;
                    if ((*j)->mCollides == true) {
                        dvector iAxesMvt = (*i)->testNormalAxes(*j);
                        if (iAxesMvt[0] == 0 && iAxesMvt[1] == 0) {
                            continue;
                        }
                        dvector jAxesMvt = (*i)->testNormalAxes(*j);
                        if (jAxesMvt[0] == 0 && jAxesMvt[1] == 0) {
                            continue;
                        }
                        
                        double iMvtSqrNorm = iAxesMvt[0] * iAxesMvt[0] + iAxesMvt[1] * iAxesMvt[1];
                        double jMvtSqrNorm = jAxesMvt[0] * jAxesMvt[0] + jAxesMvt[1] * jAxesMvt[1];
                        dvector iRvt;
                        dvector jRvt;
                        if (iMvtSqrNorm > jMvtSqrNorm) {
                            jRvt = jAxesMvt;
                            iRvt = {{-jRvt[0], -jRvt[1]}};
                        } else {
                            iRvt = iAxesMvt;
                            jRvt = {{-iRvt[0], -iRvt[1]}};
                        }
                        std::cout << "collision" << std::endl;
                        std::cout << iRvt[0] << " " << iRvt[1] << std::endl;
                        
                        
                        (*i)->resolveCollision(*j, iRvt);
                        // (*j)->resolveCollision(*i, jRvt);
                    }
                }
            }
        }
    }
    return;
}

void level :: draw(){
  // SDL_Rect rect;
  // int rgb[3];

  SDL_SetRenderTarget(_renderer, NULL);

  if(!mpSprite){
    mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1024, 1024);
  }

  SDL_RenderCopy(_renderer, mpSprite, NULL, NULL);

    //Next draw each actor
  for(auto i = mActors.begin(); i!=mActors.end(); i++){
    (*i)->draw();
  }
  /*  Draw all particles.*/
  for(auto i = mParticleList.begin(); i!= mParticleList.end(); i++){
    /* Remove the previous node if its remove flag is set */
    (*i)->draw();
  }

 return;
}
